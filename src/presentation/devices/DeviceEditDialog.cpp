#include "DeviceEditDialog.h"
#include "ui_DeviceEditDialog.h"
#include "services/DeviceService.h"
 
DeviceEditDialog::DeviceEditDialog(AppContext& ctx, QWidget* parent)
    : QDialog(parent), ui(new Ui::DeviceEditDialog), m_ctx(ctx) {
    ui->setupUi(this);
    ui->comboType->addItems({"light", "ac", "curtain", "camera", "sensor_th"});
    fillGroups();
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DeviceEditDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DeviceEditDialog::reject);
}
 
DeviceEditDialog::~DeviceEditDialog() { delete ui; }
 
void DeviceEditDialog::setDevice(const Device& device) {
    m_device = device;
    ui->editName->setText(m_device.name);
    ui->comboType->setCurrentText(m_device.type);
    int index = ui->comboGroup->findData(m_device.groupId);
    if (index != -1) ui->comboGroup->setCurrentIndex(index);
}
 
Device DeviceEditDialog::getDevice() const {
    Device d = m_device;
    d.name = ui->editName->text();
    d.type = ui->comboType->currentText();
    d.groupId = ui->comboGroup->currentData().toInt();
    return d;
}
 
void DeviceEditDialog::fillGroups() {
    ui->comboGroup->clear();
    ui->comboGroup->addItem(QString::fromUtf8("未分类"), 0);
    for (const auto& g : m_ctx.deviceService->listGroups()) {
        ui->comboGroup->addItem(g.name, g.id);
    }
}
