#include "HistoryPage.h"
#include "ui_HistoryPage.h"

#include "services/HistoryService.h"
#include "infrastructure/export/CsvExporter.h"
#include "shared/utils/Paths.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPushButton>

HistoryPage::HistoryPage(AppContext& ctx, QWidget* parent)
    : QWidget(parent), ui(new Ui::HistoryPage), m_ctx(ctx) {
    ui->setupUi(this);

    ui->tableLogs->setColumnCount(6);
    ui->tableLogs->setHorizontalHeaderLabels({"ID","时间","用户","设备","操作","结果"});
    ui->tableLogs->horizontalHeader()->setStretchLastSection(true);

    connect(ui->btnReload, &QPushButton::clicked, this, &HistoryPage::reload);
    connect(ui->btnExport, &QPushButton::clicked, this, &HistoryPage::exportCsv);

    reload();
}

HistoryPage::~HistoryPage() { delete ui; }

void HistoryPage::reload() {
  const auto rows = m_ctx.historyService->listRecent(200);
  ui->tableLogs->setRowCount(rows.size());
  for (int i=0;i<rows.size();++i) {
    const auto& r = rows[i];
    ui->tableLogs->setItem(i,0,new QTableWidgetItem(QString::number(r.id)));
    ui->tableLogs->setItem(i,1,new QTableWidgetItem(r.ts));
    ui->tableLogs->setItem(i,2,new QTableWidgetItem(r.actor));
    ui->tableLogs->setItem(i,3,new QTableWidgetItem(r.deviceName));
    ui->tableLogs->setItem(i,4,new QTableWidgetItem(r.op));
    ui->tableLogs->setItem(i,5,new QTableWidgetItem(r.result));
  }
}

void HistoryPage::exportCsv() {
  const QString dir = Paths::exportDir();
  const QString file = QFileDialog::getSaveFileName(this, "导出CSV", dir + "/operation_logs.csv", "CSV (*.csv)");
  if (file.isEmpty()) return;

  const auto rows = m_ctx.historyService->listRecent(500);

  QVector<QStringList> data;
  data.reserve(rows.size());
  for (const auto& r : rows) {
    data.push_back({QString::number(r.id), r.ts, r.actor, r.deviceName, r.op, r.result});
  }

  QString err;
  if (!CsvExporter::exportToFile(file, {"ID","时间","用户","设备","操作","结果"}, data, &err)) {
    QMessageBox::warning(this, "导出失败", err);
  } else {
    QMessageBox::information(this, "导出成功", "已导出：" + file);
  }
}
