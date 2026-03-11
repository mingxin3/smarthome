#include "SceneRepository.h"

#include <QJsonDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SceneRepository::SceneRepository(QSqlDatabase db) : m_db(db) {}

QVector<Scene> SceneRepository::listScenes() const {
  QVector<Scene> out;
  QSqlQuery q(m_db);
  q.exec("SELECT id,name,actions_json FROM scenes ORDER BY id");
  while (q.next()) {
    Scene s;
    s.id = q.value(0).toInt();
    s.name = q.value(1).toString();
    s.actions = QJsonDocument::fromJson(q.value(2).toString().toUtf8()).array();
    out.push_back(s);
  }
  return out;
}

bool SceneRepository::upsertScene(const Scene& s) {
  QSqlQuery q(m_db);
  if (s.id <= 0) {
    q.prepare("INSERT INTO scenes(name,actions_json,created_at) VALUES(?,?,datetime('now')) "
              "ON CONFLICT(name) DO UPDATE SET actions_json=excluded.actions_json");
    q.addBindValue(s.name);
    q.addBindValue(QString::fromUtf8(QJsonDocument(s.actions).toJson(QJsonDocument::Compact)));
  } else {
    q.prepare("UPDATE scenes SET name=?, actions_json=? WHERE id=?");
    q.addBindValue(s.name);
    q.addBindValue(QString::fromUtf8(QJsonDocument(s.actions).toJson(QJsonDocument::Compact)));
    q.addBindValue(s.id);
  }
  if (!q.exec()) {
    qWarning() << "upsertScene failed:" << q.lastError().text();
    return false;
  }
  return true;
}

bool SceneRepository::deleteScene(int id) {
  QSqlQuery q(m_db);
  q.prepare("DELETE FROM scenes WHERE id=?");
  q.addBindValue(id);
  return q.exec();
}
