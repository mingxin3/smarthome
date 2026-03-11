#pragma once
#include <QWidget>
#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ScenesPage; }
QT_END_NAMESPACE

class ScenesPage : public QWidget {
  Q_OBJECT
public:
  explicit ScenesPage(AppContext& ctx, QWidget* parent = nullptr);
  ~ScenesPage() override;

private:
  Ui::ScenesPage* ui;
  AppContext& m_ctx;
};