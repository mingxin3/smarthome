#include "SettingsManager.h"

SettingsManager::SettingsManager(QObject* parent)
  : QObject(parent), m_settings() {
  // defaults
  if (!m_settings.contains("alarm/temp_high")) m_settings.setValue("alarm/temp_high", 30.0);
  if (!m_settings.contains("alarm/humidity_high")) m_settings.setValue("alarm/humidity_high", 80.0);
  if (!m_settings.contains("alarm/sound_enabled")) m_settings.setValue("alarm/sound_enabled", true);
  if (!m_settings.contains("alarm/popup_enabled")) m_settings.setValue("alarm/popup_enabled", true);
  if (!m_settings.contains("system/refresh_ms")) m_settings.setValue("system/refresh_ms", 1000);
}

QVariant SettingsManager::get(const QString& key, const QVariant& def) const {
  return m_settings.value(key, def);
}

void SettingsManager::set(const QString& key, const QVariant& value) {
  m_settings.setValue(key, value);
}