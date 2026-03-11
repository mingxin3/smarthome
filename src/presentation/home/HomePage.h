#pragma once

#include <QWidget>
#include <QTimer>

#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

class EnvChartWidget;

class HomePage : public QWidget {
    Q_OBJECT
public:
    explicit HomePage(AppContext& ctx, QWidget* parent = nullptr);
    ~HomePage() override;

private slots:
    void refreshUiOnly();
    void requestGatewayPoll();

    void onAiAnalyzeClicked();

private:
    void setAiBusy(bool busy, const QString& hint = {});
    void showAiMarkdown(const QString& md);
    void showAiError(const QString& err);

private:
    Ui::HomePage* ui;
    AppContext& m_ctx;

    QTimer m_pollTimer;
    QTimer m_uiDebounceTimer;

    EnvChartWidget* m_envChart = nullptr;

    bool m_aiBusy = false;
};
