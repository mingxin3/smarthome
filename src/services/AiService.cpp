#include "AiService.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QProcessEnvironment>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

#ifdef _WIN32
#include <windows.h>
static void dbg(const QString& s) {
    OutputDebugStringW((s + "\r\n").toStdWString().c_str());
    qWarning().noquote() << s; // 同时输出到 Qt Creator 的 Application Output
}
#else
static void dbg(const QString& s) { qWarning().noquote() << s; }
#endif

AiService::AiService(Options opt, QObject* parent)
    : QObject(parent), m_opt(std::move(opt)) {
    m_nam = new QNetworkAccessManager(this);
    dbg(QString("[AI] ctor. baseUrl=%1 model=%2 timeoutMs=%3")
            .arg(m_opt.baseUrl)
            .arg(m_opt.model)
            .arg(m_opt.timeoutMs));
}

QString AiService::apiKeyFromEnv() const {
    QString key = QProcessEnvironment::systemEnvironment().value("DASHSCOPE_API_KEY").trimmed();
    if (!key.isEmpty()) return key;

    // fallback: hard-coded key (仅本地调试用，千万别提交到 GitHub)
    return QStringLiteral("");
}

QString AiService::buildPrompt(const QVector<EnvRecord>& recent,
                               double tempHigh,
                               double humidityHigh) const {
    QString s;
    s += "不要输出推理过程（reasoning），只输出最终结果。你是智能家居环境监控助手，请根据最近的环境数据判断当前状态并给出简短建议。\n\n";
    s += "要求你严格按以下 Markdown 小标题输出：\n";
    s += "## 状态概览\n";
    s += "## 阈值判断\n";
    s += "## 趋势与风险\n";
    s += "## 建议（不超过3条）\n\n";

    s += QString("阈值设定：温度高阈值 = %1 ℃，湿度高阈值 = %2 %%。\n\n")
             .arg(tempHigh, 0, 'f', 1)
             .arg(humidityHigh, 0, 'f', 1);

    s += "最近数据（按时间从旧到新，字段：ts, temperature, humidity）：\n";
    s += "```json\n[\n";

    for (int i = 0; i < recent.size(); ++i) {
        const auto& r = recent[i];
        s += QString("  {\"ts\":\"%1\",\"temperature\":%2,\"humidity\":%3}")
                 .arg(r.ts)
                 .arg(r.temperature, 0, 'f', 2)
                 .arg(r.humidity, 0, 'f', 2);
        s += (i == recent.size() - 1) ? "\n" : ",\n";
    }

    s += "]\n```\n\n";
    s += "注意：如果数据量太少或无法判断，请说明“数据不足”，并给出你需要的补充数据建议。\n";
    return s;
}

void AiService::analyzeEnv(const QVector<EnvRecord>& recent,
                           double tempHigh,
                           double humidityHigh) {
    if (m_pending) {
        dbg("[AI] analyzeEnv rejected: request already in flight");
        emit analysisFailed("AI分析正在进行中，请稍后再试。");
        return;
    }

    const QString key = apiKeyFromEnv();
    if (key.isEmpty()) {
        dbg("[AI] analyzeEnv failed: empty api key");
        emit analysisFailed("未检测到环境变量 DASHSCOPE_API_KEY。请先设置后重启程序。");
        return;
    }

    const QString prompt = buildPrompt(recent, tempHigh, humidityHigh);

    const QUrl url(m_opt.baseUrl + "/chat/completions");
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", QByteArray("Bearer ") + key.toUtf8());

    QJsonArray messages;
    messages.append(QJsonObject{{"role", "user"}, {"content", prompt}});

    QJsonObject payload{{"model", m_opt.model}, {"messages", messages}};
    const QByteArray body = QJsonDocument(payload).toJson(QJsonDocument::Compact);

    dbg(QString("[AI] POST %1  bodyBytes=%2 timeoutMs=%3 recent=%4")
            .arg(url.toString())
            .arg(body.size())
            .arg(m_opt.timeoutMs)
            .arg(recent.size()));

    m_pending = m_nam->post(req, body);
    m_pending->setProperty("abortedByTimeout", false);

    // 超时控制（调试阶段保留，但会打日志 + 标记）
    QTimer::singleShot(m_opt.timeoutMs, this, [this]{
        if (m_pending) {
            dbg(QString("[AI] timeout reached (%1ms) -> abort()").arg(m_opt.timeoutMs));
            m_pending->setProperty("abortedByTimeout", true);
            m_pending->abort();
        }
    });

    connect(m_pending, &QNetworkReply::finished, this, &AiService::onReplyFinished);
}

void AiService::onReplyFinished() {
    QNetworkReply* reply = m_pending;
    m_pending = nullptr;

    if (!reply) {
        dbg("[AI] onReplyFinished called but reply is null");
        return;
    }

    const QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    const bool abortedByTimeout = reply->property("abortedByTimeout").toBool();

    const auto err = reply->error();
    const QString errStr = reply->errorString();

    const QByteArray raw = reply->readAll();
    reply->deleteLater();

    dbg(QString("[AI] finished http=%1 err=%2 abortedByTimeout=%3 rawBytes=%4 errStr=%5")
            .arg(statusCode.toString())
            .arg(int(err))
            .arg(abortedByTimeout ? "true" : "false")
            .arg(raw.size())
            .arg(errStr));

    if (err != QNetworkReply::NoError) {
        if (err == QNetworkReply::OperationCanceledError && abortedByTimeout) {
            emit analysisFailed(QString("AI请求超时（%1ms），已取消。").arg(m_opt.timeoutMs));
        } else {
            emit analysisFailed(QString("AI请求失败：%1").arg(errStr));
        }
        if (!raw.isEmpty()) {
            dbg(QString("[AI] raw(head 2000): %1").arg(QString::fromUtf8(raw.left(2000))));
        }
        return;
    }

    dbg(QString("[AI] raw(head 2000): %1").arg(QString::fromUtf8(raw.left(2000))));

    QJsonParseError pe{};
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &pe);
    if (pe.error != QJsonParseError::NoError || !doc.isObject()) {
        emit analysisFailed(QString("AI响应解析失败：%1").arg(pe.errorString()));
        return;
    }

    const QJsonObject obj = doc.object();
    const QJsonArray choices = obj.value("choices").toArray();
    if (choices.isEmpty()) {
        emit analysisFailed("AI响应为空（choices为空）。");
        return;
    }

    const QJsonObject choice0 = choices.at(0).toObject();
    const QJsonObject message = choice0.value("message").toObject();
    const QString content = message.value("content").toString();

    if (content.trimmed().isEmpty()) {
        emit analysisFailed("AI响应内容为空。");
        return;
    }

    const QString stamped = QString("【%1】\n%2")
                                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
                                .arg(content);

    emit analysisReady(stamped);
}
