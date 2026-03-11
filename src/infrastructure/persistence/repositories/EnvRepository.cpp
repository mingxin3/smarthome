#include "EnvRepository.h"

#include <QSqlQuery>

EnvRepository::EnvRepository(QSqlDatabase db) : m_db(db) {}

QVector<EnvRecord> EnvRepository::listRecent(int limit) const {
    QVector<EnvRecord> out;
    QSqlQuery q(m_db);
    q.prepare("SELECT id,ts,IFNULL(temperature,0),IFNULL(humidity,0),IFNULL(air_quality,0) "
              "FROM env_records ORDER BY id DESC LIMIT ?");
    q.addBindValue(limit);
    q.exec();
    while (q.next()) {
        EnvRecord r;
        r.id = q.value(0).toInt();
        r.ts = q.value(1).toString();
        r.temperature = q.value(2).toDouble();
        r.humidity = q.value(3).toDouble();
        r.airQuality = q.value(4).toDouble();
        out.push_back(r);
    }
    return out;
}
