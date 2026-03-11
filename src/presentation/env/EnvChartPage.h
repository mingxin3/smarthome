#pragma once
#include <QWidget>

#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EnvChartPage; }
QT_END_NAMESPACE

class QChartView;
class EnvRepository;

class EnvChartPage : public QWidget {
    Q_OBJECT
public:
    explicit EnvChartPage(AppContext& ctx, EnvRepository& envRepo, QWidget* parent = nullptr);
    ~EnvChartPage() override;

private slots:
    void reload();

private:
    Ui::EnvChartPage* ui;
    AppContext& m_ctx;
    EnvRepository& m_envRepo;

    QChartView* m_chartView = nullptr;
};
