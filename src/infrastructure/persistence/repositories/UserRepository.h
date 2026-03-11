#pragma once
#include <QSqlDatabase>
#include <QString>

class UserRepository {
public:
  explicit UserRepository(QSqlDatabase db);

  bool validateLogin(const QString& username, const QString& passwordHash);

private:
  QSqlDatabase m_db;
};