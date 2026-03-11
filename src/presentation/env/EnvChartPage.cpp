#include "EnvChartPage.h"
#include "ui_EnvChartPage.h"

#include "infrastructure/persistence/repositories/EnvRepository.h"

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QVBoxLayout>
#include <QPainter>

EnvChartPage::EnvChartPage(AppContext& ctx, EnvRepository& envRepo, QWidget* parent)
    : QWidget(parent), ui(new Ui::EnvChartPage), m_ctx(ctx), m_envRepo(envRepo) {
    ui->setupUi(this);

    auto* layout = new QVBoxLayout(ui->chartHost);
    layout->setContentsMargins(0,0,0,0);

    m_chartView = new QChartView(new QChart(), ui->chartHost);
    m_chartView->setRenderHint(QPainter::Antialiasing, true);
    layout->addWidget(m_chartView);

    connect(ui->btnReload, &QPushButton::clicked, this, &EnvChartPage::reload);

    reload();
}

EnvChartPage::~EnvChartPage() { delete ui; }

void EnvChartPage::reload() {
    const auto rows = m_envRepo.listRecent(80);

    auto* chart = new QChart();
    chart->setTitle(QString::fromUtf8("环境曲线（最近记录）"));

    auto* tempSeries = new QLineSeries();
    tempSeries->setName(QString::fromUtf8("温度℃"));

    auto* humSeries = new QLineSeries();
    humSeries->setName(QString::fromUtf8("湿度%"));

    int x = 0;
    for (int i = rows.size() - 1; i >= 0; --i) {
        const auto& r = rows[i];
        tempSeries->append(x, r.temperature);
        humSeries->append(x, r.humidity);
        x++;
    }

    chart->addSeries(tempSeries);
    chart->addSeries(humSeries);

    auto* axisX = new QValueAxis();
    axisX->setTitleText("samples");
    axisX->setLabelFormat("%d");

    auto* axisY = new QValueAxis();
    axisY->setTitleText("value");

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    tempSeries->attachAxis(axisX);
    tempSeries->attachAxis(axisY);
    humSeries->attachAxis(axisX);
    humSeries->attachAxis(axisY);

    m_chartView->setChart(chart);
}
