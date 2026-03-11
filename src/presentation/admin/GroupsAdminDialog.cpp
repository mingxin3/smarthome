#include "GroupsAdminDialog.h"
#include "ui_GroupsAdminDialog.h"
#include "services/DeviceService.h"
#include <QInputDialog>
#include <QMessageBox>

GroupsAdminDialog::GroupsAdminDialog(AppContext& ctx, QWidget* parent)
    : QDialog(parent), ui(new Ui::GroupsAdminDialog), m_ctx(ctx) {
    ui->setupUi(this);
    connect(ui->btnAdd, &QPushButton::clicked, this, &GroupsAdminDialog::onAdd);
    connect(ui->btnEdit, &QPushButton::clicked, this, &GroupsAdminDialog::onEdit);
    connect(ui->btnDelete, &QPushButton::clicked, this, &GroupsAdminDialog::onDelete);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &GroupsAdminDialog::reject);
    reload();
}

GroupsAdminDialog::~GroupsAdminDialog() { delete ui; }

void GroupsAdminDialog::reload() {
    ui->listGroups->clear();
    const auto groups = m_ctx.deviceService->listGroups();
    for (const auto& g : groups) {
        auto* item = new QListWidgetItem(g.name);
        item->setData(Qt::UserRole, g.id);
        ui->listGroups->addItem(item);
    }
}

void GroupsAdminDialog::onAdd() {
    bool ok;
    QString name = QInputDialog::getText(this, QString::fromUtf8("添加分组"), QString::fromUtf8("名称:"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty() && m_ctx.deviceService->addGroup(name)) reload();
}

void GroupsAdminDialog::onEdit() {
    auto* item = ui->listGroups->currentItem();
    if (!item) return;
    int groupId = item->data(Qt::UserRole).toInt();
    bool ok;
    QString newName = QInputDialog::getText(this, QString::fromUtf8("编辑分组"), QString::fromUtf8("名称:"), QLineEdit::Normal, item->text(), &ok);
    if (ok && !newName.isEmpty() && m_ctx.deviceService->updateGroup(groupId, newName)) reload();
}

void GroupsAdminDialog::onDelete() {
    auto* item = ui->listGroups->currentItem();
    if (!item) return;
    if (QMessageBox::Yes == QMessageBox::question(this, QString::fromUtf8("确认"), QString::fromUtf8("确定删除该分组吗？"))) {
        if (m_ctx.deviceService->deleteGroup(item->data(Qt::UserRole).toInt())) reload();
    }
}
