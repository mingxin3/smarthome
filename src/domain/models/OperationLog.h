#pragma once
#include <QString>

struct OperationLog {
  int id = 0;
  QString ts;
  QString actor;
  int deviceId = 0;
  QString deviceName;
  QString op;
  QString result;
};