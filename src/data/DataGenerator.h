#pragma once
#include <QObject>
#include <QTimer>

class DatabaseManager;
class AlarmService;

class DataGenerator : public QObject {
  Q_OBJECT
public:
  DataGenerator(DatabaseManager& db, AlarmService& alarm, QObject* parent = nullptr);

  void start();

private slots:
  void tick();

private:
  DatabaseManager& m_db;
  AlarmService& m_alarm;
  QTimer m_timer;

  double m_temp = 24.0;
  double m_hum = 50.0;
};