#pragma once
#include "domain/models/EnvRecord.h"

#include <QSqlDatabase>
#include <QVector>

class EnvRepository {
public:
    explicit EnvRepository(QSqlDatabase db);

    QVector<EnvRecord> listRecent(int limit = 100) const;

private:
    QSqlDatabase m_db;
};
