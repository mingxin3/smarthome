#pragma once
#include <QString>

struct AlarmRecord {
  int id = 0;
  QString ts;
  QString alarmType;
  QString message;
};