#include "HistoryRepository.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

HistoryRepository::HistoryRepository(QSqlDatabase db) : m_db(db) {}

bool HistoryRepository::addOperation(const QString& actor, int deviceId, const QString& deviceName, const QString& op, const QString& result) {
  QSqlQuery q(m_db);
  q.prepare("INSERT INTO operation_logs(ts,actor,device_id,device_name,op,result) VALUES(datetime('now'),?,?,?,?,?)");
  q.addBindValue(actor);
  q.addBindValue(deviceId);
  q.addBindValue(deviceName);
  q.addBindValue(op);
  q.addBindValue(result);
  if (!q.exec()) {
    qWarning() << "addOperation failed:" << q.lastError().text();
    return false;
  }
  return true;
}

QVector<OperationLog> HistoryRepository::listRecent(int limit) const {
  QVector<OperationLog> out;
  QSqlQuery q(m_db);
  q.prepare("SELECT id,ts,actor,IFNULL(device_id,0),IFNULL(device_name,''),op,result FROM operation_logs ORDER BY id DESC LIMIT ?");
  q.addBindValue(limit);
  q.exec();
  while (q.next()) {
    OperationLog r;
    r.id = q.value(0).toInt();
    r.ts = q.value(1).toString();
    r.actor = q.value(2).toString();
    r.deviceId = q.value(3).toInt();
    r.deviceName = q.value(4).toString();
    r.op = q.value(5).toString();
    r.result = q.value(6).toString();
    out.push_back(r);
  }
  return out;
}