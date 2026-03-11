#include "Paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

QString Paths::appDataDir() {
  const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir d(base);
  d.mkpath(".");
  return d.absolutePath();
}

QString Paths::databasePath() {
  QDir d(appDataDir());
  return d.absoluteFilePath("smarthome.sqlite");
}

QString Paths::assetPath(const QString& rel) {
#ifdef APP_SOURCE_DIR
    {
        QDir sd(QString::fromUtf8(APP_SOURCE_DIR));
        const QString p = sd.absoluteFilePath("assets/" + rel);
        if (QFileInfo::exists(p)) return p;
    }
#endif

    const QString appDir = QCoreApplication::applicationDirPath();
    QDir d(appDir);

    QString p1 = d.absoluteFilePath("assets/" + rel);
    if (QFileInfo::exists(p1)) return p1;

    QString p2 = d.absoluteFilePath("../assets/" + rel);
    if (QFileInfo::exists(p2)) return p2;

    // last fallback
    return QString("assets/%1").arg(rel);
}

QString Paths::exportDir() {
  QDir d(appDataDir());
  d.mkpath("export");
  return d.absoluteFilePath("export");
}
