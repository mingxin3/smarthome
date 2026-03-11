#pragma once
#include <QObject>
#include <QString>

class SceneRepository;
class DeviceService;
class HistoryRepository;

class SceneService : public QObject {
    Q_OBJECT
public:
    SceneService(SceneRepository& repo, DeviceService& device, HistoryRepository& history, QObject* parent = nullptr);

    // 激活指定场景（按名字）
    bool activateScene(const QString& actor, const QString& sceneName, QString* err = nullptr);

    void regenerateDefaultScenes();

private:
    SceneRepository& m_repo;
    DeviceService& m_device;
    HistoryRepository& m_history;
};
