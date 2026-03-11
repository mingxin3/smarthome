#pragma once

#include <QObject>

class AlarmRepository;
class SoundPlayer;
class SettingsManager;

class AlarmService : public QObject {
  Q_OBJECT
public:
  AlarmService(AlarmRepository& repo, SoundPlayer& sound, SettingsManager& settings, QObject* parent = nullptr);

  void onEnvData(double temperature, double humidity);

signals:
  void alarmRaised(QString message);

private:
  AlarmRepository& m_repo;
  SoundPlayer& m_sound;
  SettingsManager& m_settings;

  bool m_lastTempAlarm = false;
  bool m_lastHumAlarm = false;
};