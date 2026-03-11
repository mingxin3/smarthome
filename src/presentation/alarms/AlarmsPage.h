#pragma once
#include <QWidget>
#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AlarmsPage; }
QT_END_NAMESPACE

class AlarmsPage : public QWidget {
  Q_OBJECT
public:
  explicit AlarmsPage(AppContext& ctx, QWidget* parent = nullptr);
  ~AlarmsPage() override;

private slots:
  void loadSettings();
  void saveSettings();
  void reloadTable();

private:
  Ui::AlarmsPage* ui;
  AppContext& m_ctx;
};