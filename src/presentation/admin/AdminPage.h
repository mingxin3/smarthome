#pragma once
#include <QWidget>
#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AdminPage; }
QT_END_NAMESPACE

class AdminPage : public QWidget {
  Q_OBJECT
public:
  explicit AdminPage(AppContext& ctx, QWidget* parent = nullptr);
  ~AdminPage() override;

private slots:
  void load();
  void save();
  void backup();

private:
  Ui::AdminPage* ui;
  AppContext& m_ctx;
};