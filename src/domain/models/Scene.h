#pragma once
#include <QString>
#include <QJsonArray>

struct Scene {
  int id = 0;
  QString name;
  QJsonArray actions; // array of action objects
};