#include "DevicesPage.h"
#include "ui_DevicesPage.h"

#include "services/DeviceService.h"
#include "presentation/admin/GroupsAdminDialog.h"
#include "presentation/devices/DeviceEditDialog.h"
#include "presentation/widgets/SwitchButton.h"

#include <QMessageBox>
#include <QListWidgetItem>
#include <QJsonObject>
#include <QTreeWidgetItem>

static bool getDeviceById(const QVector<Device>& devices, int id, Device& out) {
    for (const auto& d : devices) {
        if (d.id == id) { out = d; return true; }
    }
    return false;
}

DevicesPage::DevicesPage(AppContext& ctx, QWidget* parent)
    : QWidget(parent), ui(new Ui::DevicesPage), m_ctx(ctx) {
    ui->setupUi(this);

    connect(ui->sliderValue, &QSlider::valueChanged, this, [this](int v){
        // 文本会在 fillControlsFromDevice 里按设备类型覆盖；这里保底显示
        ui->lblValue->setText(QString::fromUtf8("参数值：%1").arg(v));
    });

    connect(ui->treeDevices, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem*, QTreeWidgetItem*){ onSelectDevice(); });
    connect(ui->btnApply, &QPushButton::clicked, this, &DevicesPage::applyControl);
    connect(ui->btnManageGroups, &QPushButton::clicked, this, &DevicesPage::openGroupsAdmin);
    connect(ui->btnAddDevice, &QPushButton::clicked, this, &DevicesPage::onAddDevice);
    connect(ui->btnEditDevice, &QPushButton::clicked, this, &DevicesPage::onEditDevice);
    connect(ui->btnDeleteDevice, &QPushButton::clicked, this, &DevicesPage::onDeleteDevice);

    m_toggle = new SwitchButton(this);
    m_toggle->setFixedSize(54, 32);
    ui->verticalLayout->insertWidget(2, m_toggle);
    ui->chkOn->hide();

    // 合并刷新（UI 侧 100ms）
    m_refreshTimer.setSingleShot(true);
    m_refreshTimer.setInterval(100);
    connect(&m_refreshTimer, &QTimer::timeout, this, &DevicesPage::refreshVisibleItems);

    // 监听批量更新信号（如果你还没替换 DeviceService，也可以先改回 deviceStateUpdated）
    connect(m_ctx.deviceService.get(), &DeviceService::devicesBatchUpdated, this, [this]{
        scheduleRefresh();
    });

    reloadFull();
    m_ctx.deviceService->requestRefreshFromGateway();
}

DevicesPage::~DevicesPage() { delete ui; }

void DevicesPage::scheduleRefresh() {
    if (!m_refreshTimer.isActive()) m_refreshTimer.start();
}

QString DevicesPage::makeDeviceText(const Device& d) const {
    const QString onlineText = d.online ? QString::fromUtf8("🟢在线") : QString::fromUtf8("🔴离线");
    QString base = QString::fromUtf8("#%1 %2 [%3] %4")
        .arg(d.id)
        .arg(d.name)
        .arg(d.type)
        .arg(onlineText);
    if (d.type == "light" || d.type == "ac") {
        const bool on = d.state.value("on").toBool(false);
        base += QString::fromUtf8(" ") + (on ? QString::fromUtf8("✅") : QString::fromUtf8("❌"));
    }
    return base;
}

void DevicesPage::reloadFull() {
    const auto devices = m_ctx.deviceService->listDevices();
    const auto groups = m_ctx.deviceService->listGroups();
    rebuildTree(groups, devices);
}

void DevicesPage::refreshVisibleItems() {
    const auto devices = m_ctx.deviceService->listDevices();

    ui->treeDevices->setUpdatesEnabled(false);
    for (const auto& d : devices) {
        if (m_itemById.contains(d.id)) {
            m_itemById[d.id]->setText(0, makeDeviceText(d));
        }
    }
    ui->treeDevices->setUpdatesEnabled(true);

    // 若当前选中设备有更新，同步回填控件
    if (m_selectedDeviceId > 0) {
        Device cur;
        if (getDeviceById(devices, m_selectedDeviceId, cur)) {
            fillControlsFromDevice(cur);
        }
    }
}

void DevicesPage::fillControlsFromDevice(const Device& d) {
    const QJsonObject st = d.state;

    if (d.type == "light") {
        m_toggle->setVisible(true);
        ui->sliderValue->setMinimum(0);
        ui->sliderValue->setMaximum(100);

        m_toggle->setChecked(st.value("on").toBool(false));
        const int br = st.value("brightness").toInt(50);
        ui->sliderValue->setValue(qBound(0, br, 100));
        ui->lblValue->setText(QString::fromUtf8("亮度：%1").arg(ui->sliderValue->value()));
    } else if (d.type == "ac") {
        m_toggle->setVisible(true);
        ui->sliderValue->setMinimum(16);
        ui->sliderValue->setMaximum(30);

        m_toggle->setChecked(st.value("on").toBool(false));
        const int temp = st.value("temp").toInt(26);
        ui->sliderValue->setValue(qBound(16, temp, 30));
        ui->lblValue->setText(QString::fromUtf8("温度：%1℃").arg(ui->sliderValue->value()));
    } else if (d.type == "curtain") {
        m_toggle->setVisible(false);

        ui->sliderValue->setMinimum(0);
        ui->sliderValue->setMaximum(100);

        const int pos = st.value("position").toInt(0);
        ui->sliderValue->setValue(qBound(0, pos, 100));
        ui->lblValue->setText(QString::fromUtf8("开合：%1%").arg(ui->sliderValue->value()));
    } else {
        m_toggle->setVisible(false);
        ui->sliderValue->setMinimum(0);
        ui->sliderValue->setMaximum(100);
        ui->sliderValue->setValue(qBound(0, st.value("value").toInt(50), 100));
        ui->lblValue->setText(QString::fromUtf8("参数值：%1").arg(ui->sliderValue->value()));
    }
}

QJsonObject DevicesPage::buildPatchForDevice(const Device& d) const {
    QJsonObject patch;

    if (d.type == "light") {
        patch["on"] = m_toggle->isChecked();
        patch["brightness"] = ui->sliderValue->value();
    } else if (d.type == "ac") {
        patch["on"] = m_toggle->isChecked();
        patch["temp"] = ui->sliderValue->value();
        patch["mode"] = d.state.value("mode").toString("cool");
    } else if (d.type == "curtain") {
        patch["position"] = ui->sliderValue->value();
    } else {
        patch["value"] = ui->sliderValue->value();
    }

    return patch;
}

void DevicesPage::onSelectDevice() {
    auto* it = ui->treeDevices->currentItem();
    if (!it || it->data(0, Qt::UserRole).toInt() <= 0) {
        m_selectedDeviceId = 0;
        ui->lblSelected->setText(QString::fromUtf8("未选择设备"));
        return;
    }

    m_selectedDeviceId = it->data(0, Qt::UserRole).toInt();
    ui->lblSelected->setText(QString::fromUtf8("已选择设备：%1").arg(it->text(0)));

    // 切换设备立即回填控件（独立控制）
    const auto devices = m_ctx.deviceService->listDevices();
    Device cur;
    if (getDeviceById(devices, m_selectedDeviceId, cur)) {
        fillControlsFromDevice(cur);
    }
}

void DevicesPage::applyControl() {
    if (m_selectedDeviceId <= 0) {
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请先在左侧选择一个设备。"));
        return;
    }

    const auto devices = m_ctx.deviceService->listDevices();
    Device cur;
    if (!getDeviceById(devices, m_selectedDeviceId, cur)) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("未找到设备数据，请刷新后重试。"));
        return;
    }

    const QJsonObject patch = buildPatchForDevice(cur);
    m_ctx.deviceService->controlDevice("admin", m_selectedDeviceId, patch);

    scheduleRefresh();
}

void DevicesPage::rebuildTree(const QVector<DeviceGroup>& groups, const QVector<Device>& devices) {
    ui->treeDevices->setUpdatesEnabled(false);
    ui->treeDevices->clear();
    m_itemById.clear();
    m_groupItemById.clear();

    for (const auto& g : groups) {
        auto* grp = new QTreeWidgetItem(QStringList() << g.name);
        grp->setData(0, Qt::UserRole, -g.id);
        ui->treeDevices->addTopLevelItem(grp);
        m_groupItemById.insert(g.id, grp);
    }
    if (!m_groupItemById.contains(0)) {
        auto* grp = new QTreeWidgetItem(QStringList() << QString::fromUtf8("未分类"));
        grp->setData(0, Qt::UserRole, 0);
        ui->treeDevices->addTopLevelItem(grp);
        m_groupItemById.insert(0, grp);
    }

    for (const auto& d : devices) {
        auto* parent = m_groupItemById.value(d.groupId, m_groupItemById.value(0));
        auto* it = new QTreeWidgetItem(QStringList() << makeDeviceText(d));
        it->setData(0, Qt::UserRole, d.id);
        parent->addChild(it);
        m_itemById.insert(d.id, it);
    }

    ui->treeDevices->expandAll();
    ui->treeDevices->setUpdatesEnabled(true);
    if (!m_itemById.isEmpty() && m_selectedDeviceId == 0) {
        ui->treeDevices->setCurrentItem(m_itemById.constBegin().value());
    }
}

bool DevicesPage::currentSelectedDeviceId(int& outId) const {
    auto* it = ui->treeDevices->currentItem();
    if (!it) return false;
    int id = it->data(0, Qt::UserRole).toInt();
    if (id <= 0) return false;
    outId = id;
    return true;
}

void DevicesPage::openGroupsAdmin() {
    GroupsAdminDialog dlg(m_ctx, this);
    dlg.exec();
    const auto devices = m_ctx.deviceService->listDevices();
    const auto groups = m_ctx.deviceService->listGroups();
    rebuildTree(groups, devices);
}

void DevicesPage::onAddDevice() {
    DeviceEditDialog dlg(m_ctx, this);
    Device d;
    d.name = QString::fromUtf8("新设备");
    d.type = "light";
    d.groupId = 0;
    d.online = true;
    d.state = QJsonObject();
    dlg.setDevice(d);
    if (dlg.exec() == QDialog::Accepted) {
        Device nd = dlg.getDevice();
        if (m_ctx.deviceService->addDevice(nd)) {
            const auto devices = m_ctx.deviceService->listDevices();
            const auto groups = m_ctx.deviceService->listGroups();
            rebuildTree(groups, devices);
        }
    }
}

void DevicesPage::onEditDevice() {
    int id;
    if (!currentSelectedDeviceId(id)) return;
    const auto devices = m_ctx.deviceService->listDevices();
    Device cur;
    if (!getDeviceById(devices, id, cur)) return;
    DeviceEditDialog dlg(m_ctx, this);
    dlg.setDevice(cur);
    if (dlg.exec() == QDialog::Accepted) {
        Device nd = dlg.getDevice();
        nd.id = id;
        if (m_ctx.deviceService->updateDevice(nd)) {
            const auto devs = m_ctx.deviceService->listDevices();
            const auto groups = m_ctx.deviceService->listGroups();
            rebuildTree(groups, devs);
        }
    }
}

void DevicesPage::onDeleteDevice() {
    int id;
    if (!currentSelectedDeviceId(id)) return;
    if (QMessageBox::Yes == QMessageBox::question(this, QString::fromUtf8("确认"), QString::fromUtf8("确定删除该设备吗？"))) {
        if (m_ctx.deviceService->deleteDevice(id)) {
            const auto devices = m_ctx.deviceService->listDevices();
            const auto groups = m_ctx.deviceService->listGroups();
            rebuildTree(groups, devices);
        }
    }
}
