#pragma once
#include <QObject>
#include <QSettings>
#include <QVariant>

class SettingsManager : public QObject {
  Q_OBJECT
public:
  explicit SettingsManager(QObject* parent = nullptr);

  QVariant get(const QString& key, const QVariant& def = {}) const;
  void set(const QString& key, const QVariant& value);

private:
  QSettings m_settings;
};