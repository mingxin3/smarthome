#pragma once

#include <QWidget>
#include <QTimer>
#include <QHash>

#include "app/AppContext.h"
#include "domain/models/Device.h"   // 关键：补上 Device 定义
#include "domain/models/DeviceGroup.h"
class SwitchButton;

QT_BEGIN_NAMESPACE
namespace Ui { class DevicesPage; }
QT_END_NAMESPACE

class QListWidgetItem;
class QTreeWidgetItem;

class DevicesPage : public QWidget {
    Q_OBJECT
public:
    explicit DevicesPage(AppContext& ctx, QWidget* parent = nullptr);
    ~DevicesPage() override;

private slots:
    void reloadFull();
    void scheduleRefresh();
    void refreshVisibleItems();
    void onSelectDevice();
    void applyControl();
    void openGroupsAdmin();
    void onAddDevice();
    void onEditDevice();
    void onDeleteDevice();

private:
    void fillControlsFromDevice(const Device& d);
    QJsonObject buildPatchForDevice(const Device& d) const;
    QString makeDeviceText(const Device& d) const;
    void rebuildTree(const QVector<DeviceGroup>& groups, const QVector<Device>& devices);
    bool currentSelectedDeviceId(int& outId) const;

    Ui::DevicesPage* ui;
    AppContext& m_ctx;
    SwitchButton* m_toggle = nullptr;

    int m_selectedDeviceId = 0;

    QHash<int, QTreeWidgetItem*> m_itemById;
    QHash<int, QTreeWidgetItem*> m_groupItemById;
    QTimer m_refreshTimer;
};
