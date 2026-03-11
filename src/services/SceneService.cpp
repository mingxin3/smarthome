#include "SceneService.h"

#include "infrastructure/persistence/repositories/SceneRepository.h"
#include "services/DeviceService.h"
#include "infrastructure/persistence/repositories/HistoryRepository.h"
#include "domain/models/Scene.h"

#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>

SceneService::SceneService(SceneRepository& repo, DeviceService& device, HistoryRepository& history, QObject* parent)
    : QObject(parent), m_repo(repo), m_device(device), m_history(history) {}

bool SceneService::activateScene(const QString& actor, const QString& sceneName, QString* err) {
    const auto scenes = m_repo.listScenes();
    for (const auto& s : scenes) {
        if (s.name == sceneName) {
            // actions: [{deviceId:1, state:{...}}, ...]
            for (const auto& v : s.actions) {
                const QJsonObject obj = v.toObject();
                const int deviceId = obj.value("deviceId").toInt();
                const QJsonObject state = obj.value("state").toObject();
                if (deviceId > 0 && !state.isEmpty()) {
                    m_device.controlDevice(actor, deviceId, state);
                }
            }
            m_history.addOperation(actor, 0, "", "activate_scene", sceneName);
            return true;
        }
    }

    if (err) *err = QString::fromUtf8("未找到场景：") + sceneName;
    return false;
}

void SceneService::regenerateDefaultScenes() {
    const auto devices = m_device.listDevices();

    QJsonArray home;
    QJsonArray sleep;
    QJsonArray away;

    struct Entry { int id; QJsonObject st; };
    QVector<Entry> eHome, eSleep, eAway;

    for (const auto& d : devices) {
        if (d.type == "light") {
            eHome.push_back({d.id, QJsonObject{{"on", true}, {"brightness", 80}}});
            eSleep.push_back({d.id, QJsonObject{{"on", false}, {"brightness", 0}}});
            eAway.push_back({d.id, QJsonObject{{"on", false}, {"brightness", 0}}});
        } else if (d.type == "ac") {
            eHome.push_back({d.id, QJsonObject{{"on", true}, {"temp", 26}, {"mode", "cool"}}});
            eSleep.push_back({d.id, QJsonObject{{"on", true}, {"temp", 28}, {"mode", "sleep"}}});
            eAway.push_back({d.id, QJsonObject{{"on", false}}});
        } else if (d.type == "curtain") {
            eHome.push_back({d.id, QJsonObject{{"position", 80}}});
            eSleep.push_back({d.id, QJsonObject{{"position", 0}}});
            eAway.push_back({d.id, QJsonObject{{"position", 0}}});
        }
    }

    auto toArray = [](QVector<Entry>& v) {
        std::sort(v.begin(), v.end(), [](const Entry& a, const Entry& b){ return a.id < b.id; });
        QJsonArray arr;
        for (const auto& e : v) {
            QJsonObject obj;
            obj["deviceId"] = e.id;
            obj["state"] = e.st;
            arr.push_back(obj);
        }
        return arr;
    };

    home = toArray(eHome);
    sleep = toArray(eSleep);
    away = toArray(eAway);

    Scene s1; s1.name = QString::fromUtf8("回家模式"); s1.actions = home;
    Scene s2; s2.name = QString::fromUtf8("睡眠模式"); s2.actions = sleep;
    Scene s3; s3.name = QString::fromUtf8("离家模式"); s3.actions = away;

    m_repo.upsertScene(s1);
    m_repo.upsertScene(s2);
    m_repo.upsertScene(s3);
}
