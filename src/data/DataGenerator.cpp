#include "DataGenerator.h"

#include "infrastructure/persistence/DatabaseManager.h"
#include "services/AlarmService.h"

#include <QSqlQuery>
#include <QRandomGenerator>

DataGenerator::DataGenerator(DatabaseManager& db, AlarmService& alarm, QObject* parent)
  : QObject(parent), m_db(db), m_alarm(alarm) {
  connect(&m_timer, &QTimer::timeout, this, &DataGenerator::tick);
}

void DataGenerator::start() {
  m_timer.start(4000); // 4s
}

void DataGenerator::tick() {
  // random walk
  m_temp += (QRandomGenerator::global()->bounded(100) - 50) / 100.0; // [-0.5,0.5]
  m_hum += (QRandomGenerator::global()->bounded(100) - 50) / 80.0;   // smaller drift
  double air = 50 + QRandomGenerator::global()->bounded(50);

  // sometimes push higher to show alarm
  if (QRandomGenerator::global()->bounded(20) == 0) m_temp += 3.5;
  if (QRandomGenerator::global()->bounded(25) == 0) m_hum += 10.0;

  QSqlQuery q(m_db.database());
  q.prepare("INSERT INTO env_records(ts,temperature,humidity,air_quality) VALUES(datetime('now'),?,?,?)");
  q.addBindValue(m_temp);
  q.addBindValue(m_hum);
  q.addBindValue(air);
  q.exec();

  m_alarm.onEnvData(m_temp, m_hum);
}
