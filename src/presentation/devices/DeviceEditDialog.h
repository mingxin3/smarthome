#pragma once
#include <QDialog>
#include "app/AppContext.h"
#include "domain/models/Device.h"
 
QT_BEGIN_NAMESPACE
namespace Ui { class DeviceEditDialog; }
QT_END_NAMESPACE
 
class DeviceEditDialog : public QDialog {
    Q_OBJECT
public:
    explicit DeviceEditDialog(AppContext& ctx, QWidget* parent = nullptr);
    ~DeviceEditDialog() override;
    void setDevice(const Device& device);
    Device getDevice() const;
private:
    void fillGroups();
    Ui::DeviceEditDialog* ui;
    AppContext& m_ctx;
    Device m_device;
};
