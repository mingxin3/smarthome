#pragma once
#include <QWidget>
#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HistoryPage; }
QT_END_NAMESPACE

class HistoryPage : public QWidget {
  Q_OBJECT
public:
  explicit HistoryPage(AppContext& ctx, QWidget* parent = nullptr);
  ~HistoryPage() override;

private slots:
  void reload();
  void exportCsv();

private:
  Ui::HistoryPage* ui;
  AppContext& m_ctx;
};