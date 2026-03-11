#include "AlarmService.h"
#include "infrastructure/persistence/repositories/AlarmRepository.h"
#include "infrastructure/audio/SoundPlayer.h"
#include "infrastructure/settings/SettingsManager.h"
#include "infrastructure/settings/SettingsKeys.h"
#include "domain/enums/AlarmType.h"

#include <QString>

AlarmService::AlarmService(AlarmRepository& repo, SoundPlayer& sound, SettingsManager& settings, QObject* parent)
  : QObject(parent), m_repo(repo), m_sound(sound), m_settings(settings) {}

void AlarmService::onEnvData(double temperature, double humidity) {
  const double tempHigh = m_settings.get(SettingsKeys::AlarmTempHigh, 30.0).toDouble();
  const double humHigh = m_settings.get(SettingsKeys::AlarmHumidityHigh, 80.0).toDouble();

  const bool tempAlarm = temperature > tempHigh;
  const bool humAlarm = humidity > humHigh;

  if (tempAlarm && !m_lastTempAlarm) {
    const QString msg = QString("温度过高：%1℃ > %2℃").arg(temperature, 0, 'f', 1).arg(tempHigh, 0, 'f', 1);
    m_repo.addAlarm(toString(AlarmType::EnvThreshold), msg);
    if (m_settings.get(SettingsKeys::AlarmSoundEnabled, true).toBool()) m_sound.playAlarm();
    emit alarmRaised(msg);
  }
  if (humAlarm && !m_lastHumAlarm) {
    const QString msg = QString("湿度过高：%1%% > %2%%").arg(humidity, 0, 'f', 1).arg(humHigh, 0, 'f', 1);
    m_repo.addAlarm(toString(AlarmType::EnvThreshold), msg);
    if (m_settings.get(SettingsKeys::AlarmSoundEnabled, true).toBool()) m_sound.playAlarm();
    emit alarmRaised(msg);
  }

  m_lastTempAlarm = tempAlarm;
  m_lastHumAlarm = humAlarm;
}