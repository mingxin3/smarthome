#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager() = default;
DatabaseManager::~DatabaseManager() {
  if (m_db.isValid()) m_db.close();
}

bool DatabaseManager::open(const QString& filePath) {
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(filePath);
  if (!m_db.open()) {
    qWarning() << "DB open failed:" << m_db.lastError().text();
    return false;
  }
  return true;
}

QSqlDatabase DatabaseManager::database() const { return m_db; }

bool DatabaseManager::isInitialized() const {
  QSqlQuery q(m_db);
  if (!q.exec("CREATE TABLE IF NOT EXISTS app_meta(key TEXT PRIMARY KEY, value TEXT NOT NULL)")) return false;
  q.prepare("SELECT value FROM app_meta WHERE key='initialized'");
  if (!q.exec()) return false;
  if (q.next()) return q.value(0).toString() == "1";
  return false;
}

void DatabaseManager::markInitialized() {
  QSqlQuery q(m_db);
  q.exec("CREATE TABLE IF NOT EXISTS app_meta(key TEXT PRIMARY KEY, value TEXT NOT NULL)");
  q.prepare("INSERT OR REPLACE INTO app_meta(key,value) VALUES('initialized','1')");
  q.exec();
}