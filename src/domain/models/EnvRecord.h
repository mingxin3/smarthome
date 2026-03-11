#pragma once
#include <QString>

struct EnvRecord {
  int id = 0;
  QString ts;
  double temperature = 0;
  double humidity = 0;
  double airQuality = 0;
};