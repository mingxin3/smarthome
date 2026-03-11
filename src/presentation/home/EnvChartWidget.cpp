#include "EnvChartWidget.h"
#include "ui_EnvChartWidget.h"

#include "infrastructure/persistence/repositories/EnvRepository.h"
#include "infrastructure/settings/SettingsKeys.h"
#include "infrastructure/settings/SettingsManager.h"

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QVBoxLayout>
#include <QPainter>
#include <QtMath>

static double clampMinSpan(double span, double minSpan) {
    return (span < minSpan) ? minSpan : span;
}

EnvChartWidget::EnvChartWidget(AppContext& ctx, EnvRepository& envRepo, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::EnvChartWidget)
    , m_ctx(ctx)
    , m_envRepo(envRepo) {

    ui->setupUi(this);

    auto* layout = new QVBoxLayout(ui->chartHost);
    layout->setContentsMargins(0, 0, 0, 0);

    m_chartView = new QChartView(ui->chartHost);
    m_chartView->setRenderHint(QPainter::Antialiasing, true);
    layout->addWidget(m_chartView);

    ensureChartBuilt();

    // 6s refresh
    m_refreshTimer.setInterval(6000);
    connect(&m_refreshTimer, &QTimer::timeout, this, &EnvChartWidget::reload);
    m_refreshTimer.start();

    reload();
}

EnvChartWidget::~EnvChartWidget() {
    delete ui;
}

void EnvChartWidget::ensureChartBuilt() {
    if (m_chart) return;

    m_chart = new QChart();
    m_chart->setTitle(QString::fromUtf8("环境实时曲线（温度/湿度，6s更新）"));
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);

    // ---------- 数据曲线（配色：温度橙色、湿度蓝色） ----------
    m_tempSeries = new QLineSeries();
    m_tempSeries->setName(QString::fromUtf8("温度℃"));

    m_humSeries = new QLineSeries();
    m_humSeries->setName(QString::fromUtf8("湿度%"));

    {
        QPen p(QColor(255, 140, 0)); // dark orange
        p.setWidth(2);
        m_tempSeries->setPen(p);
    }
    {
        QPen p(QColor(30, 144, 255)); // dodger blue
        p.setWidth(2);
        m_humSeries->setPen(p);
    }

    // ---------- 阈值线（不同颜色 + 虚线不同强度） ----------
    // 温度阈值：红色偏深，虚线更粗
    m_tempThSeries = new QLineSeries();
    m_tempThSeries->setName(QString::fromUtf8("温度阈值（alarm/temp_high）"));
    {
        QPen p(QColor(220, 20, 60));  // crimson
        p.setStyle(Qt::DashLine);
        p.setWidth(3);               // stronger
        m_tempThSeries->setPen(p);
    }

    // 湿度阈值：紫色（或洋红），虚线略细
    m_humThSeries = new QLineSeries();
    m_humThSeries->setName(QString::fromUtf8("湿度阈值（alarm/humidity_high）"));
    {
        QPen p(QColor(138, 43, 226)); // blue violet
        p.setStyle(Qt::DashLine);
        p.setWidth(2);               // weaker
        m_humThSeries->setPen(p);
    }

    m_chart->addSeries(m_tempSeries);
    m_chart->addSeries(m_humSeries);
    m_chart->addSeries(m_tempThSeries);
    m_chart->addSeries(m_humThSeries);

    // ---------- 坐标轴 ----------
    m_axisX = new QValueAxis();
    m_axisX->setTitleText("samples");
    m_axisX->setLabelFormat("%d");
    m_axisX->setTickCount(9);

    m_axisTemp = new QValueAxis();
    m_axisTemp->setTitleText(QString::fromUtf8("温度℃"));

    m_axisHum = new QValueAxis();
    m_axisHum->setTitleText(QString::fromUtf8("湿度%"));

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisTemp, Qt::AlignLeft);
    m_chart->addAxis(m_axisHum, Qt::AlignRight);

    // Attach: temp -> left, hum -> right
    m_tempSeries->attachAxis(m_axisX);
    m_tempSeries->attachAxis(m_axisTemp);

    m_humSeries->attachAxis(m_axisX);
    m_humSeries->attachAxis(m_axisHum);

    m_tempThSeries->attachAxis(m_axisX);
    m_tempThSeries->attachAxis(m_axisTemp);

    m_humThSeries->attachAxis(m_axisX);
    m_humThSeries->attachAxis(m_axisHum);

    m_chartView->setChart(m_chart);
}

void EnvChartWidget::updateThresholdSeries(int pointCount, double tempHigh, double humHigh) {
    // horizontal lines across whole x-range
    const int n = qMax(2, pointCount);
    const int xMax = n - 1;

    m_tempThSeries->clear();
    m_humThSeries->clear();

    m_tempThSeries->append(0, tempHigh);
    m_tempThSeries->append(xMax, tempHigh);

    m_humThSeries->append(0, humHigh);
    m_humThSeries->append(xMax, humHigh);
}

void EnvChartWidget::reload() {
    ensureChartBuilt();

    // 阈值来自 alarm 配置；同时给一个“系统刚启动/没配置”时的合理默认值
    const double tempHigh = (m_ctx.settings ? m_ctx.settings->get(SettingsKeys::AlarmTempHigh, 30.0).toDouble() : 30.0);
    const double humHigh  = (m_ctx.settings ? m_ctx.settings->get(SettingsKeys::AlarmHumidityHigh, 80.0).toDouble() : 80.0);

    const auto rows = m_envRepo.listRecent(m_limit);
    const int n = rows.size();

    // --------- 无数据：给一个“合理的初始化显示”，避免空图/轴异常 ---------
    if (n <= 0) {
        // 让图表看起来“正常”，给出固定范围（你也可以按产品需求调整）
        // 温度：20~35℃，湿度：40~90%
        m_tempSeries->clear();
        m_humSeries->clear();

        // 同时把阈值线也画出来（即使没有数据��
        updateThresholdSeries(2, tempHigh, humHigh);

        m_axisX->setRange(0, 1);
        m_axisTemp->setRange(20.0, 35.0);
        m_axisHum->setRange(40.0, 90.0);

        return;
    }

    // --------- 有数据：正常绘制 ---------
    m_tempSeries->clear();
    m_humSeries->clear();

    double tempMin =  1e9, tempMax = -1e9;
    double humMin  =  1e9, humMax  = -1e9;

    // rows is ORDER BY id DESC -> reverse to old->new
    int x = 0;
    for (int i = n - 1; i >= 0; --i) {
        const auto& r = rows[i];

        m_tempSeries->append(x, r.temperature);
        m_humSeries->append(x, r.humidity);

        tempMin = qMin(tempMin, r.temperature);
        tempMax = qMax(tempMax, r.temperature);

        humMin = qMin(humMin, r.humidity);
        humMax = qMax(humMax, r.humidity);

        x++;
    }

    // 让阈值线一定可见
    tempMin = qMin(tempMin, tempHigh);
    tempMax = qMax(tempMax, tempHigh);
    humMin  = qMin(humMin, humHigh);
    humMax  = qMax(humMax, humHigh);

    // --------- 让“曲线幅度更显著”的轴策略 ---------
    // 之前你觉得“Y轴坐标太大、幅度不显著”，常见原因是 padding/span 太大。
    // 这里改为：
    // - padding 取 span 的 8%，但同时给一个最小 padding（温度 0.5℃、湿度 2%）
    // - 同时限制最小 span（温度至少 5℃、湿度至少 20%），避免数据全一样时轴贴死导致不好看
    const double tempSpan = clampMinSpan(tempMax - tempMin, 5.0);
    const double humSpan  = clampMinSpan(humMax - humMin, 20.0);

    const double tempPad = qMax(0.5, tempSpan * 0.08);
    const double humPad  = qMax(2.0, humSpan * 0.08);

    // 以中点为基准扩展，保证最终 span 至少为 tempSpan/humSpan
    const double tempMid = (tempMin + tempMax) * 0.5;
    const double humMid  = (humMin + humMax) * 0.5;

    const double tempHalf = (tempSpan * 0.5) + tempPad;
    const double humHalf  = (humSpan * 0.5) + humPad;

    m_axisX->setRange(0, qMax(1, n - 1));
    m_axisTemp->setRange(tempMid - tempHalf, tempMid + tempHalf);
    m_axisHum->setRange(humMid - humHalf, humMid + humHalf);

    updateThresholdSeries(n, tempHigh, humHigh);
}
