#include "AdminPage.h"
#include "ui_AdminPage.h"

#include "services/AdminService.h"
#include "shared/utils/Paths.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

AdminPage::AdminPage(AppContext& ctx, QWidget* parent)
  : QWidget(parent), ui(new Ui::AdminPage), m_ctx(ctx) {
  ui->setupUi(this);
  connect(ui->btnSave, &QPushButton::clicked, this, &AdminPage::save);
  connect(ui->btnBackup, &QPushButton::clicked, this, &AdminPage::backup);

  load();
}

AdminPage::~AdminPage() { delete ui; }

void AdminPage::load() {
  ui->spinRefresh->setValue(m_ctx.adminService->refreshMs());
}

void AdminPage::save() {
  m_ctx.adminService->setRefreshMs(ui->spinRefresh->value());
  QMessageBox::information(this, "保存", "已保存。");
}

void AdminPage::backup() {
  const QString dir = Paths::appDataDir();
  const QString defaultName = "smarthome_backup_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".sqlite";
  const QString file = QFileDialog::getSaveFileName(this, "备份数据库", dir + "/" + defaultName, "SQLite (*.sqlite *.db)");
  if (file.isEmpty()) return;

  QString err;
  if (!m_ctx.adminService->backupDatabase(file, &err)) {
    QMessageBox::warning(this, "备份失败", err);
  } else {
    QMessageBox::information(this, "备份成功", "已备份到：" + file);
  }
}