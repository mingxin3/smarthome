#include "AlarmsPage.h"
#include "ui_AlarmsPage.h"

#include "infrastructure/settings/SettingsManager.h"
#include "infrastructure/settings/SettingsKeys.h"
#include "infrastructure/persistence/repositories/AlarmRepository.h"

AlarmsPage::AlarmsPage(AppContext& ctx, QWidget* parent)
  : QWidget(parent), ui(new Ui::AlarmsPage), m_ctx(ctx) {
  ui->setupUi(this);

  ui->tableAlarms->setColumnCount(4);
  ui->tableAlarms->setHorizontalHeaderLabels({"ID","时间","类型","内容"});
  ui->tableAlarms->horizontalHeader()->setStretchLastSection(true);

  connect(ui->btnSave, &QPushButton::clicked, this, &AlarmsPage::saveSettings);
  connect(ui->btnReload, &QPushButton::clicked, this, &AlarmsPage::reloadTable);

  loadSettings();
  reloadTable();
}

AlarmsPage::~AlarmsPage() { delete ui; }

void AlarmsPage::loadSettings() {
  ui->spinTemp->setValue(m_ctx.settings->get(SettingsKeys::AlarmTempHigh, 30.0).toDouble());
  ui->spinHum->setValue(m_ctx.settings->get(SettingsKeys::AlarmHumidityHigh, 80.0).toDouble());
  ui->chkSound->setChecked(m_ctx.settings->get(SettingsKeys::AlarmSoundEnabled, true).toBool());
  ui->chkPopup->setChecked(m_ctx.settings->get(SettingsKeys::AlarmPopupEnabled, true).toBool());
}

void AlarmsPage::saveSettings() {
  m_ctx.settings->set(SettingsKeys::AlarmTempHigh, ui->spinTemp->value());
  m_ctx.settings->set(SettingsKeys::AlarmHumidityHigh, ui->spinHum->value());
  m_ctx.settings->set(SettingsKeys::AlarmSoundEnabled, ui->chkSound->isChecked());
  m_ctx.settings->set(SettingsKeys::AlarmPopupEnabled, ui->chkPopup->isChecked());
}

void AlarmsPage::reloadTable() {
  const auto rows = m_ctx.alarmRepo->listRecent(200);
  ui->tableAlarms->setRowCount(rows.size());
  for (int i=0;i<rows.size();++i) {
    const auto& r = rows[i];
    ui->tableAlarms->setItem(i,0,new QTableWidgetItem(QString::number(r.id)));
    ui->tableAlarms->setItem(i,1,new QTableWidgetItem(r.ts));
    ui->tableAlarms->setItem(i,2,new QTableWidgetItem(r.alarmType));
    ui->tableAlarms->setItem(i,3,new QTableWidgetItem(r.message));
  }
}