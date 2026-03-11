#pragma once
#include <QString>

class DeviceRepository;
class SceneRepository;
class SettingsManager;
class DatabaseManager;

class AdminService {
public:
  AdminService(DeviceRepository& dev, SceneRepository& scene, SettingsManager& settings, DatabaseManager& db);

  void setRefreshMs(int ms);
  int refreshMs() const;

  bool backupDatabase(const QString& dstFile, QString* err = nullptr);

private:
  DeviceRepository& m_dev;
  SceneRepository& m_scene;
  SettingsManager& m_settings;
  DatabaseManager& m_db;
};