#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include "domain/models/EnvRecord.h"

class QNetworkAccessManager;
class QNetworkReply;

class AiService : public QObject {
    Q_OBJECT
public:
    struct Options {
        QString baseUrl;
        QString model;
        int timeoutMs;

        Options()
            : baseUrl("https://dashscope.aliyuncs.com/compatible-mode/v1"),
            model("qwen3.5-plus"),
            timeoutMs(60000) {}
    };

    explicit AiService(Options opt = Options(), QObject* parent = nullptr);

    void analyzeEnv(const QVector<EnvRecord>& recent,
                    double tempHigh,
                    double humidityHigh);

signals:
    void analysisReady(QString markdown);
    void analysisFailed(QString error);

private slots:
    void onReplyFinished();

private:
    QString buildPrompt(const QVector<EnvRecord>& recent,
                        double tempHigh,
                        double humidityHigh) const;

    QString apiKeyFromEnv() const;

private:
    Options m_opt;
    QNetworkAccessManager* m_nam = nullptr;
    QNetworkReply* m_pending = nullptr;
};
