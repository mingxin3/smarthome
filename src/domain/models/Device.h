#pragma once
#include <QString>
#include <QJsonObject>

struct Device {
  int id = 0;
  QString name;
  QString type;          // stored string (light/ac/curtain/camera/sensor_th)
  int groupId = 0;
  bool online = false;
  QJsonObject state;     // {"on":true,...}
};