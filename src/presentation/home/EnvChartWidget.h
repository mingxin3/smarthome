#pragma once

#include <QWidget>
#include <QTimer>

#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EnvChartWidget; }
QT_END_NAMESPACE

class QChartView;
class QChart;
class QLineSeries;
class QValueAxis;

class EnvRepository;

class EnvChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit EnvChartWidget(AppContext& ctx, EnvRepository& envRepo, QWidget* parent = nullptr);
    ~EnvChartWidget() override;

private slots:
    void reload();

private:
    void ensureChartBuilt();
    void updateThresholdSeries(int pointCount, double tempHigh, double humHigh);

    Ui::EnvChartWidget* ui;
    AppContext& m_ctx;
    EnvRepository& m_envRepo;

    QTimer m_refreshTimer;

    QChartView* m_chartView = nullptr;
    QChart* m_chart = nullptr;

    QLineSeries* m_tempSeries = nullptr;
    QLineSeries* m_humSeries = nullptr;

    // threshold series (red dashed)
    QLineSeries* m_tempThSeries = nullptr;
    QLineSeries* m_humThSeries = nullptr;

    QValueAxis* m_axisX = nullptr;
    QValueAxis* m_axisTemp = nullptr; // left
    QValueAxis* m_axisHum = nullptr;  // right

    int m_limit = 80;
};
