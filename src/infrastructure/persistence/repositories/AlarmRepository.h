#pragma once
#include "domain/models/AlarmRecord.h"

#include <QSqlDatabase>
#include <QVector>

class AlarmRepository {
public:
  explicit AlarmRepository(QSqlDatabase db);

  bool addAlarm(const QString& alarmType, const QString& message);
  QVector<AlarmRecord> listRecent(int limit = 200) const;
  int countRecent(int limit = 50) const;

private:
  QSqlDatabase m_db;
};