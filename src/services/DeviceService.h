#pragma once
#include "domain/models/Device.h"
#include "domain/models/DeviceGroup.h"

#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QTimer>
#include <QHash>

class DeviceRepository;
class HistoryRepository;
class DeviceGatewayClient;

class DeviceService : public QObject {
    Q_OBJECT
public:
    DeviceService(DeviceRepository& repo, HistoryRepository& history, DeviceGatewayClient& client, QObject* parent = nullptr);

    QVector<Device> listDevices() const;
    QVector<DeviceGroup> listGroups() const;

    void requestRefreshFromGateway();
    void controlDevice(const QString& actor, int deviceId, const QJsonObject& patchState);

    bool addGroup(const QString& name);
    bool updateGroup(int id, const QString& name);
    bool deleteGroup(int id);

    bool addDevice(const Device& d);
    bool updateDevice(const Device& d);
    bool deleteDevice(int id);

signals:
    void deviceStateUpdated(int deviceId);
    void devicesBatchUpdated();
    void groupsChanged();
    void devicesSchemaChanged();

private slots:
    void onGatewayMessage(QJsonObject msg);
    void flushPendingStates();

private:
    void scheduleFlush();

    DeviceRepository& m_repo;
    HistoryRepository& m_history;
    DeviceGatewayClient& m_client;

    struct PendingState {
        bool online = true;
        QJsonObject state;
    };
    QHash<int, PendingState> m_pending;
    QTimer m_flushTimer;

    QTimer m_refreshThrottle;
    bool m_refreshRequestedWhileThrottled = false;
};
