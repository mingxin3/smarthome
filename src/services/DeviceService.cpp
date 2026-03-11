#include "DeviceService.h"

#include "infrastructure/persistence/repositories/DeviceRepository.h"
#include "infrastructure/persistence/repositories/HistoryRepository.h"
#include "infrastructure/network/DeviceGatewayClient.h"
#include <QSqlQuery>

DeviceService::DeviceService(DeviceRepository& repo, HistoryRepository& history, DeviceGatewayClient& client, QObject* parent)
    : QObject(parent), m_repo(repo), m_history(history), m_client(client) {

    connect(&m_client, &DeviceGatewayClient::messageReceived, this, &DeviceService::onGatewayMessage);

    // 100ms 合并落库一次
    m_flushTimer.setSingleShot(true);
    m_flushTimer.setInterval(100);
    connect(&m_flushTimer, &QTimer::timeout, this, &DeviceService::flushPendingStates);

    // 刷新限流：500ms 内最多一次 getAll
    m_refreshThrottle.setSingleShot(true);
    m_refreshThrottle.setInterval(500);
    connect(&m_refreshThrottle, &QTimer::timeout, this, [this]{
        if (m_refreshRequestedWhileThrottled) {
            m_refreshRequestedWhileThrottled = false;
            requestRefreshFromGateway();
        }
    });
}

QVector<Device> DeviceService::listDevices() const {
    return m_repo.listDevices();
}

QVector<DeviceGroup> DeviceService::listGroups() const {
    return m_repo.listGroups();
}

void DeviceService::requestRefreshFromGateway() {
    if (m_refreshThrottle.isActive()) {
        m_refreshRequestedWhileThrottled = true;
        return;
    }
    m_refreshThrottle.start();

    QJsonObject req;
    req["type"] = "getAll";
    m_client.send(req);
}

void DeviceService::controlDevice(const QString& actor, int deviceId, const QJsonObject& patchState) {
    // 注意：这里发送的是“patch state”，由模拟网关直接写入 state
    QJsonObject cmd;
    cmd["type"] = "set";
    cmd["deviceId"] = deviceId;
    cmd["state"] = patchState;
    m_client.send(cmd);

    m_history.addOperation(actor, deviceId, QString("device#%1").arg(deviceId), "control", "sent");
}

void DeviceService::scheduleFlush() {
    if (!m_flushTimer.isActive())
        m_flushTimer.start();
}

void DeviceService::onGatewayMessage(QJsonObject msg) {
    const QString type = msg.value("type").toString();
    if (type == "state") {
        const int id = msg.value("deviceId").toInt();
        const bool online = msg.value("online").toBool(true);
        const QJsonObject state = msg.value("state").toObject();

        PendingState p;
        p.online = online;
        p.state = state;
        m_pending.insert(id, p);

        scheduleFlush();
    }
}

void DeviceService::flushPendingStates() {
    if (m_pending.isEmpty()) return;

    for (auto it = m_pending.begin(); it != m_pending.end(); ++it) {
        const int id = it.key();
        const PendingState& p = it.value();
        m_repo.updateDeviceState(id, p.state, p.online);
        emit deviceStateUpdated(id); // 兼容旧逻辑
    }
    m_pending.clear();

    emit devicesBatchUpdated();
}

bool DeviceService::addGroup(const QString& name) {
    bool ok = m_repo.addGroup(name);
    if (ok) emit groupsChanged();
    return ok;
}

bool DeviceService::updateGroup(int id, const QString& name) {
    bool ok = m_repo.updateGroup(id, name);
    if (ok) emit groupsChanged();
    return ok;
}

bool DeviceService::deleteGroup(int id) {
    bool ok = m_repo.deleteGroup(id);
    if (ok) emit groupsChanged();
    return ok;
}

bool DeviceService::addDevice(const Device& d) {
    bool ok = m_repo.addDevice(d);
    if (ok) {
        emit devicesSchemaChanged();
        emit devicesBatchUpdated();
    }
    return ok;
}

bool DeviceService::updateDevice(const Device& d) {
    bool ok = m_repo.updateDevice(d);
    if (ok) {
        emit devicesSchemaChanged();
        emit devicesBatchUpdated();
    }
    return ok;
}

bool DeviceService::deleteDevice(int id) {
    bool ok = m_repo.deleteDevice(id);
    if (ok) {
        emit devicesSchemaChanged();
        emit devicesBatchUpdated();
    }
    return ok;
}
