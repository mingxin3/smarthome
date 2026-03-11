#include "DeviceRepository.h"

#include <QJsonDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DeviceRepository::DeviceRepository(QSqlDatabase db) : m_db(db) {}

QVector<Device> DeviceRepository::listDevices() const {
  QVector<Device> out;
  QSqlQuery q(m_db);
  q.exec("SELECT id,name,type,IFNULL(group_id,0),online,state_json FROM devices ORDER BY id");
  while (q.next()) {
    Device d;
    d.id = q.value(0).toInt();
    d.name = q.value(1).toString();
    d.type = q.value(2).toString();
    d.groupId = q.value(3).toInt();
    d.online = q.value(4).toInt() == 1;
    const QByteArray json = q.value(5).toString().toUtf8();
    d.state = QJsonDocument::fromJson(json).object();
    out.push_back(d);
  }
  return out;
}

bool DeviceRepository::updateDeviceState(int deviceId, const QJsonObject& state, bool online) {
  QSqlQuery q(m_db);
  q.prepare("UPDATE devices SET state_json=?, online=? WHERE id=?");
  q.addBindValue(QString::fromUtf8(QJsonDocument(state).toJson(QJsonDocument::Compact)));
  q.addBindValue(online ? 1 : 0);
  q.addBindValue(deviceId);
  if (!q.exec()) {
    qWarning() << "updateDeviceState failed:" << q.lastError().text();
    return false;
  }
  return true;
}

bool DeviceRepository::setAllOnline(bool online) {
  QSqlQuery q(m_db);
  q.prepare("UPDATE devices SET online=?");
  q.addBindValue(online ? 1 : 0);
  return q.exec();
}

int DeviceRepository::countDevices() const {
  QSqlQuery q(m_db);
  q.exec("SELECT COUNT(*) FROM devices");
  if (q.next()) return q.value(0).toInt();
  return 0;
}

int DeviceRepository::countOnlineDevices() const {
  QSqlQuery q(m_db);
  q.exec("SELECT COUNT(*) FROM devices WHERE online=1");
  if (q.next()) return q.value(0).toInt();
  return 0;
}

QVector<DeviceGroup> DeviceRepository::listGroups() const {
  QVector<DeviceGroup> out;
  QSqlQuery q(m_db);
  q.exec("SELECT id,name FROM device_groups ORDER BY id");
  while (q.next()) {
    DeviceGroup g;
    g.id = q.value(0).toInt();
    g.name = q.value(1).toString();
    out.push_back(g);
  }
  return out;
}

bool DeviceRepository::addGroup(const QString& name) {
  QSqlQuery q(m_db);
  q.prepare("INSERT INTO device_groups(name) VALUES(?)");
  q.addBindValue(name);
  if (!q.exec()) {
    qWarning() << "addGroup failed:" << q.lastError().text();
    return false;
  }
  return true;
}

bool DeviceRepository::updateGroup(int id, const QString& name) {
  QSqlQuery q(m_db);
  q.prepare("UPDATE device_groups SET name=? WHERE id=?");
  q.addBindValue(name);
  q.addBindValue(id);
  if (!q.exec()) {
    qWarning() << "updateGroup failed:" << q.lastError().text();
    return false;
  }
  return true;
}

bool DeviceRepository::deleteGroup(int id) {
  QSqlQuery q(m_db);
  q.prepare("DELETE FROM device_groups WHERE id=?");
  q.addBindValue(id);
  if (!q.exec()) {
    qWarning() << "deleteGroup failed:" << q.lastError().text();
    return false;
  }
  return true;
}

bool DeviceRepository::addDevice(const Device& d) {
  QSqlQuery q(m_db);
  q.prepare("INSERT INTO devices(name,type,group_id,online,state_json,created_at) VALUES(?,?,?,?,?,datetime('now'))");
  q.addBindValue(d.name);
  q.addBindValue(d.type);
  q.addBindValue(d.groupId);
  q.addBindValue(d.online ? 1 : 0);
  q.addBindValue(QString::fromUtf8(QJsonDocument(d.state).toJson(QJsonDocument::Compact)));
  if (!q.exec()) {
    qWarning() << "addDevice failed:" << q.lastError().text();
    return false;
  }
  return true;
}

bool DeviceRepository::updateDevice(const Device& d) {
  QSqlQuery q(m_db);
  q.prepare("UPDATE devices SET name=?, type=?, group_id=?, online=?, state_json=? WHERE id=?");
  q.addBindValue(d.name);
  q.addBindValue(d.type);
  q.addBindValue(d.groupId);
  q.addBindValue(d.online ? 1 : 0);
  q.addBindValue(QString::fromUtf8(QJsonDocument(d.state).toJson(QJsonDocument::Compact)));
  q.addBindValue(d.id);
  if (!q.exec()) {
    qWarning() << "updateDevice failed:" << q.lastError().text();
    return false;
  }
  return true;
}

bool DeviceRepository::deleteDevice(int id) {
  QSqlQuery q(m_db);
  q.prepare("DELETE FROM devices WHERE id=?");
  q.addBindValue(id);
  if (!q.exec()) {
    qWarning() << "deleteDevice failed:" << q.lastError().text();
    return false;
  }
  return true;
}
