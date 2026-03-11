#pragma once

#include <QString>

class Paths {
public:
  static QString appDataDir();                 // writable
  static QString databasePath();               // appDataDir()/smarthome.sqlite
  static QString assetPath(const QString& rel); // assets/db/...
  static QString exportDir();                  // appDataDir()/export
};