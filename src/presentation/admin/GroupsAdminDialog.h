#pragma once
#include <QDialog>
#include "app/AppContext.h"
 
QT_BEGIN_NAMESPACE
namespace Ui { class GroupsAdminDialog; }
QT_END_NAMESPACE
 
class GroupsAdminDialog : public QDialog {
    Q_OBJECT
public:
    explicit GroupsAdminDialog(AppContext& ctx, QWidget* parent = nullptr);
    ~GroupsAdminDialog() override;
private slots:
    void reload();
    void onAdd();
    void onEdit();
    void onDelete();
private:
    Ui::GroupsAdminDialog* ui;
    AppContext& m_ctx;
};
