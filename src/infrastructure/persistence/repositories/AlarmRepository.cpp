#include "AlarmRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

AlarmRepository::AlarmRepository(QSqlDatabase db) : m_db(db) {}

bool AlarmRepository::addAlarm(const QString& alarmType, const QString& message) {
  QSqlQuery q(m_db);
  q.prepare("INSERT INTO alarm_records(ts,alarm_type,message) VALUES(datetime('now'),?,?)");
  q.addBindValue(alarmType);
  q.addBindValue(message);
  if (!q.exec()) {
    qWarning() << "addAlarm failed:" << q.lastError().text();
    return false;
  }
  return true;
}

QVector<AlarmRecord> AlarmRepository::listRecent(int limit) const {
  QVector<AlarmRecord> out;
  QSqlQuery q(m_db);
  q.prepare("SELECT id,ts,alarm_type,message FROM alarm_records ORDER BY id DESC LIMIT ?");
  q.addBindValue(limit);
  q.exec();
  while (q.next()) {
    AlarmRecord r;
    r.id = q.value(0).toInt();
    r.ts = q.value(1).toString();
    r.alarmType = q.value(2).toString();
    r.message = q.value(3).toString();
    out.push_back(r);
  }
  return out;
}

int AlarmRepository::countRecent(int limit) const {
  QSqlQuery q(m_db);
  q.prepare("SELECT COUNT(*) FROM (SELECT id FROM alarm_records ORDER BY id DESC LIMIT ?)");
  q.addBindValue(limit);
  q.exec();
  if (q.next()) return q.value(0).toInt();
  return 0;
}