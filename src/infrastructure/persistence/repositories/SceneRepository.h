#pragma once
#include "domain/models/Scene.h"

#include <QSqlDatabase>
#include <QVector>

class SceneRepository {
public:
  explicit SceneRepository(QSqlDatabase db);

  QVector<Scene> listScenes() const;
  bool upsertScene(const Scene& s);
  bool deleteScene(int id);

private:
  QSqlDatabase m_db;
};