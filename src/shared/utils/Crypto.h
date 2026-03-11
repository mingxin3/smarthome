#pragma once

#include <QString>

class Crypto {
public:
  static QString hashPassword(const QString& plain);
};