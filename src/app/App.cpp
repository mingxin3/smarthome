#include "App.h"

#include "shared/utils/Paths.h"
#include "shared/utils/Crypto.h"

#include "infrastructure/persistence/DatabaseManager.h"
#include "infrastructure/settings/SettingsManager.h"
#include "infrastructure/audio/SoundPlayer.h"

#include "infrastructure/persistence/repositories/UserRepository.h"
#include "infrastructure/persistence/repositories/DeviceRepository.h"
#include "infrastructure/persistence/repositories/SceneRepository.h"
#include "infrastructure/persistence/repositories/HistoryRepository.h"
#include "infrastructure/persistence/repositories/AlarmRepository.h"
#include "infrastructure/persistence/repositories/EnvRepository.h"

#include "infrastructure/network/ProtocolCodec.h"
#include "infrastructure/network/DeviceGatewayClient.h"
#include "infrastructure/network/SimulatedDeviceServer.h"

#include "services/AuthService.h"
#include "services/DeviceService.h"
#include "services/SceneService.h"
#include "services/HistoryService.h"
#include "services/AlarmService.h"
#include "services/AdminService.h"
#include "services/AiService.h"

#include "data/DataGenerator.h"

#include "presentation/navigation/MainWindow.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

App::App(QObject* parent) : QObject(parent), m_ctx(std::make_unique<AppContext>()) {}
App::~App() = default;

AppContext& App::ctx() { return *m_ctx; }

static bool ensureDir(const QString& path) {
    QDir d(path);
    if (d.exists()) return true;
    return d.mkpath(".");
}

static bool execSqlFile(QSqlDatabase db, const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open sql file:" << filePath;
        return false;
    }
    const QString sqlAll = QString::fromUtf8(f.readAll());
    const QStringList statements = sqlAll.split(";", Qt::SkipEmptyParts);
    QSqlQuery q(db);
    for (const QString& stRaw : statements) {
        const QString st = stRaw.trimmed();
        if (st.isEmpty()) continue;
        if (!q.exec(st)) {
            qWarning() << "SQL failed:" << q.lastError().text() << "\nStatement:\n" << st;
            return false;
        }
    }
    return true;
}

bool App::init() {
    // Paths
    const QString dataDir = Paths::appDataDir();
    if (!ensureDir(dataDir)) return false;

    // Settings
    m_ctx->settings = std::make_unique<SettingsManager>();

    // Database
    m_ctx->db = std::make_unique<DatabaseManager>();
    const QString dbPath = Paths::databasePath();
    if (!m_ctx->db->open(dbPath)) return false;

    // First run init (schema + seed)
    if (!m_ctx->db->isInitialized()) {
        const QString schema = Paths::assetPath("db/schema.sql");
        const QString seed = Paths::assetPath("db/seed.sql");

        if (!execSqlFile(m_ctx->db->database(), schema)) return false;
        if (!execSqlFile(m_ctx->db->database(), seed)) return false;

        // Fix admin password hash placeholder
        {
            QSqlQuery q(m_ctx->db->database());
            q.prepare("UPDATE users SET password_hash=? WHERE username='admin' AND password_hash='TEMP_PLACEHOLDER'");
            q.addBindValue(Crypto::hashPassword("admin"));
            if (!q.exec()) {
                qWarning() << "Update admin password hash failed:" << q.lastError().text();
                return false;
            }
        }

        m_ctx->db->markInitialized();
    }

    // Repositories
    m_ctx->userRepo = std::make_unique<UserRepository>(m_ctx->db->database());
    m_ctx->deviceRepo = std::make_unique<DeviceRepository>(m_ctx->db->database());
    m_ctx->sceneRepo = std::make_unique<SceneRepository>(m_ctx->db->database());
    m_ctx->historyRepo = std::make_unique<HistoryRepository>(m_ctx->db->database());
    m_ctx->alarmRepo = std::make_unique<AlarmRepository>(m_ctx->db->database());
    m_ctx->envRepo = std::make_unique<EnvRepository>(m_ctx->db->database());

    // Audio
    m_ctx->sound = std::make_unique<SoundPlayer>();

    // Network codec + client
    m_ctx->codec = std::make_unique<ProtocolCodec>();
    m_ctx->gatewayClient = std::make_unique<DeviceGatewayClient>(*m_ctx->codec);

    // Simulated device server (in-process)
    m_ctx->simulatedServer = std::make_unique<SimulatedDeviceServer>(*m_ctx->codec, *m_ctx->deviceRepo);
    if (!m_ctx->simulatedServer->start(QHostAddress::LocalHost, 45454)) {
        qWarning() << "Simulated server start failed";
        return false;
    }
    m_ctx->gatewayClient->connectToHost("127.0.0.1", 45454);

    // Services
    m_ctx->authService = std::make_unique<AuthService>(*m_ctx->userRepo);
    m_ctx->deviceService = std::make_unique<DeviceService>(*m_ctx->deviceRepo, *m_ctx->historyRepo, *m_ctx->gatewayClient);
    m_ctx->sceneService = std::make_unique<SceneService>(*m_ctx->sceneRepo, *m_ctx->deviceService, *m_ctx->historyRepo);
    m_ctx->historyService = std::make_unique<HistoryService>(*m_ctx->historyRepo, *m_ctx->deviceRepo);
    m_ctx->alarmService = std::make_unique<AlarmService>(*m_ctx->alarmRepo, *m_ctx->sound, *m_ctx->settings);
    m_ctx->adminService = std::make_unique<AdminService>(*m_ctx->deviceRepo, *m_ctx->sceneRepo, *m_ctx->settings, *m_ctx->db);

    // AI service (Qwen compatible-mode)
    {
        AiService::Options opt;
        opt.baseUrl = "https://dashscope.aliyuncs.com/compatible-mode/v1";
        opt.model = "qwen3.5-plus";
        opt.timeoutMs = 30000;
        m_ctx->aiService = std::make_unique<AiService>(opt);
    }

    // Data generator (env data + alarm trigger demo)
    m_ctx->dataGen = std::make_unique<DataGenerator>(*m_ctx->db, *m_ctx->alarmService);
    m_ctx->dataGen->start();

    QObject::connect(m_ctx->deviceService.get(), &DeviceService::devicesSchemaChanged, m_ctx->sceneService.get(), &SceneService::regenerateDefaultScenes);
    m_ctx->sceneService->regenerateDefaultScenes();

    return true;
}

QWidget* App::createMainWindow() {
    auto* w = new MainWindow(*m_ctx);
    w->setWindowTitle("智能家居监控平台（Base）");
    w->resize(1100, 720);
    return w;
}
