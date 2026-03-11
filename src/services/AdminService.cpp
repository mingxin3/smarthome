#include "AdminService.h"

#include "infrastructure/persistence/DatabaseManager.h"
#include "infrastructure/settings/SettingsManager.h"
#include "infrastructure/settings/SettingsKeys.h"

#include <QFile>

AdminService::AdminService(DeviceRepository& dev, SceneRepository& scene, SettingsManager& settings, DatabaseManager& db)
  : m_dev(dev), m_scene(scene), m_settings(settings), m_db(db) {}

void AdminService::setRefreshMs(int ms) {
  m_settings.set(SettingsKeys::RefreshMs, ms);
}

int AdminService::refreshMs() const {
  return m_settings.get(SettingsKeys::RefreshMs, 1000).toInt();
}

bool AdminService::backupDatabase(const QString& dstFile, QString* err) {
  const QString src = m_db.database().databaseName();
  if (!QFile::copy(src, dstFile)) {
    if (err) *err = "备份失败（无法复制数据库文件）";
    return false;
  }
  return true;
}