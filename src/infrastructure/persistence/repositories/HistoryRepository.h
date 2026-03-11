#pragma once
#include "domain/models/OperationLog.h"

#include <QSqlDatabase>
#include <QVector>

class HistoryRepository {
public:
  explicit HistoryRepository(QSqlDatabase db);

  bool addOperation(const QString& actor, int deviceId, const QString& deviceName, const QString& op, const QString& result);
  QVector<OperationLog> listRecent(int limit = 200) const;

private:
  QSqlDatabase m_db;
};