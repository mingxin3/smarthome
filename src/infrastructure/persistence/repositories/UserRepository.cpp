#include "UserRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

UserRepository::UserRepository(QSqlDatabase db) : m_db(db) {}

bool UserRepository::validateLogin(const QString& username, const QString& passwordHash) {
  QSqlQuery q(m_db);
  q.prepare("SELECT 1 FROM users WHERE username=? AND password_hash=?");
  q.addBindValue(username);
  q.addBindValue(passwordHash);
  if (!q.exec()) {
    qWarning() << "validateLogin failed:" << q.lastError().text();
    return false;
  }
  return q.next();
}