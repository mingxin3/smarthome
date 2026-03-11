#pragma once

#include "AppContext.h"

#include <QObject>
#include <memory>

class QWidget;

class App : public QObject {
  Q_OBJECT

public:
  explicit App(QObject* parent = nullptr);
  ~App() override;

  bool init();
  QWidget* createMainWindow();

  AppContext& ctx();

private:
  std::unique_ptr<AppContext> m_ctx;
};