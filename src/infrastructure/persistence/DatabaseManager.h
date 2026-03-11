#pragma once

#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
  DatabaseManager();
  ~DatabaseManager();

  bool open(const QString& filePath);
  QSqlDatabase database() const;

  bool isInitialized() const;
  void markInitialized();

private:
  QSqlDatabase m_db;
};