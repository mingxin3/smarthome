#pragma once
#include "domain/models/Device.h"
#include "domain/models/DeviceGroup.h"

#include <QSqlDatabase>
#include <QVector>
#include <QString>
#include <QJsonObject>

class DeviceRepository {
public:
  explicit DeviceRepository(QSqlDatabase db);

  QVector<Device> listDevices() const;
  bool updateDeviceState(int deviceId, const QJsonObject& state, bool online);
  bool setAllOnline(bool online);

  int countDevices() const;
  int countOnlineDevices() const;

  QVector<DeviceGroup> listGroups() const;
  bool addGroup(const QString& name);
  bool updateGroup(int id, const QString& name);
  bool deleteGroup(int id);

  bool addDevice(const Device& d);
  bool updateDevice(const Device& d);
  bool deleteDevice(int id);

private:
  QSqlDatabase m_db;
};
