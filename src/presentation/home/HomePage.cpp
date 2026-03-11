#include "HomePage.h"
#include "ui_HomePage.h"
#pragma message("Using ui_HomePage.h from: " __FILE__)
#include "presentation/home/EnvChartWidget.h"

#include "services/DeviceService.h"
#include "services/AiService.h"

#include "infrastructure/persistence/repositories/DeviceRepository.h"
#include "infrastructure/persistence/repositories/AlarmRepository.h"
#include "infrastructure/persistence/repositories/EnvRepository.h"

#include "infrastructure/settings/SettingsKeys.h"
#include "infrastructure/settings/SettingsManager.h"

#include <QVBoxLayout>
#include <QDateTime>

HomePage::HomePage(AppContext& ctx, QWidget* parent)
    : QWidget(parent), ui(new Ui::HomePage), m_ctx(ctx) {
    ui->setupUi(this);

    // 嵌入实时曲线（EnvRepository 由 AppContext 注入）
    if (m_ctx.envRepo) {
        auto* hostLayout = new QVBoxLayout(ui->envChartHost);
        hostLayout->setContentsMargins(0, 0, 0, 0);

        m_envChart = new EnvChartWidget(m_ctx, *m_ctx.envRepo, ui->envChartHost);
        hostLayout->addWidget(m_envChart);
    }

    // 手动刷新：只刷新 UI，并发起一次网关刷新
    connect(ui->btnRefresh, &QPushButton::clicked, this, [this]{
        refreshUiOnly();
        m_ctx.deviceService->requestRefreshFromGateway();
    });

    // AI 按钮
    connect(ui->btnAiAnalyze, &QPushButton::clicked, this, &HomePage::onAiAnalyzeClicked);

    // 订阅 AI 服务回调
    if (m_ctx.aiService) {
        connect(m_ctx.aiService.get(), &AiService::analysisReady, this, [this](const QString& md){
            setAiBusy(false);
            showAiMarkdown(md);
        });
        connect(m_ctx.aiService.get(), &AiService::analysisFailed, this, [this](const QString& err){
            setAiBusy(false);
            showAiError(err);
        });
    } else {
        ui->btnAiAnalyze->setEnabled(false);
        ui->txtAiResult->setText("AI服务未初始化。");
    }

    // deviceStateUpdated 可能很频繁：只做 UI 合并刷新，不要每次都 requestRefreshFromGateway
    m_uiDebounceTimer.setSingleShot(true);
    m_uiDebounceTimer.setInterval(150);
    connect(&m_uiDebounceTimer, &QTimer::timeout, this, &HomePage::refreshUiOnly);

    connect(m_ctx.deviceService.get(), &DeviceService::deviceStateUpdated, this, [this](int){
        if (!m_uiDebounceTimer.isActive()) m_uiDebounceTimer.start();
    });

    // 定时轮询网关（避免 HomePage 内形成刷新风暴）
    m_pollTimer.setInterval(8000); // 8s
    connect(&m_pollTimer, &QTimer::timeout, this, &HomePage::requestGatewayPoll);
    m_pollTimer.start();

    refreshUiOnly();
}

HomePage::~HomePage() { delete ui; }

void HomePage::refreshUiOnly() {
    ui->lblDevices->setText(QString::fromUtf8("设备总数：%1").arg(m_ctx.deviceRepo->countDevices()));
    ui->lblOnline->setText(QString::fromUtf8("在线设备：%1").arg(m_ctx.deviceRepo->countOnlineDevices()));
    ui->lblAlarms->setText(QString::fromUtf8("最近告警：%1").arg(m_ctx.alarmRepo->countRecent(50)));
}

void HomePage::requestGatewayPoll() {
    m_ctx.deviceService->requestRefreshFromGateway();
}

void HomePage::setAiBusy(bool busy, const QString& hint) {
    m_aiBusy = busy;
    ui->btnAiAnalyze->setEnabled(!busy);
    if (busy) {
        const QString msg = hint.isEmpty() ? "正在分析中，请稍候..." : hint;
        ui->txtAiResult->setHtml(QString(
                                     "<div style='color:#666'>%1</div>"
                                     "<div style='color:#999;font-size:12px;margin-top:6px'>提示：若长时间无响应，请检查网络或 DASHSCOPE_API_KEY。</div>"
                                     ).arg(msg.toHtmlEscaped()));
    }
}

void HomePage::showAiMarkdown(const QString& md) {
    // QTextBrowser 不���生渲染 Markdown（Qt 6.4+ 有 setMarkdown，但不同版本差异）
    // 为兼容性：直接做“等宽 + 换行保留”的简单展示（稳定）。
    // 如果你 Qt 版本支持 setMarkdown，可以把这段改成 ui->txtAiResult->setMarkdown(md);
    const QString html = QString(
                             "<div style='font-family:Segoe UI, Microsoft YaHei;'>"
                             "<div style='white-space:pre-wrap;'>%1</div>"
                             "<div style='color:#999;font-size:12px;margin-top:8px;'>更新时间：%2</div>"
                             "</div>"
                             ).arg(md.toHtmlEscaped())
                             .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    ui->txtAiResult->setHtml(html);
}

void HomePage::showAiError(const QString& err) {
    ui->txtAiResult->setHtml(QString(
                                 "<div style='color:#b00020;font-weight:600'>AI分析失败</div>"
                                 "<div style='white-space:pre-wrap;margin-top:6px;color:#444'>%1</div>"
                                 "<div style='color:#999;font-size:12px;margin-top:8px'>请检查：网络是否可用、DASHSCOPE_API_KEY 是否已设置、是否需要代理。</div>"
                                 ).arg(err.toHtmlEscaped()));
}

void HomePage::onAiAnalyzeClicked() {
    if (!m_ctx.aiService || !m_ctx.envRepo) {
        showAiError("AI服务或环境数据仓库未初始化。");
        return;
    }
    if (m_aiBusy) return;

    // 取最近数据（建议 30 条，避免 prompt 太长）
    auto recent = m_ctx.envRepo->listRecent(30);
    // listRecent 是 DESC -> 这里反转成 old->new，和图表一致，也更便于模型理解
    std::reverse(recent.begin(), recent.end());

    const double tempHigh = m_ctx.settings ? m_ctx.settings->get(SettingsKeys::AlarmTempHigh, 30.0).toDouble() : 30.0;
    const double humHigh  = m_ctx.settings ? m_ctx.settings->get(SettingsKeys::AlarmHumidityHigh, 80.0).toDouble() : 80.0;

    setAiBusy(true);
    m_ctx.aiService->analyzeEnv(recent, tempHigh, humHigh);
}
