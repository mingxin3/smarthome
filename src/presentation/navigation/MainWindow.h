#pragma once

#include <QMainWindow>
#include <memory>

#include "app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LoginPage;
class HomePage;
class DevicesPage;
class ScenesPage;
class HistoryPage;
class AlarmsPage;
class AdminPage;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(AppContext& ctx, QWidget *parent = nullptr);
  ~MainWindow() override;

private slots:
  void onLoggedIn(QString username);
  void logout();

private:
  void showLogin();
  void showAppPages();

  Ui::MainWindow *ui;
  AppContext& m_ctx;

  QString m_currentUser;

  LoginPage* m_login = nullptr;
  HomePage* m_home = nullptr;
  DevicesPage* m_devices = nullptr;
  ScenesPage* m_scenes = nullptr;
  HistoryPage* m_history = nullptr;
  AlarmsPage* m_alarms = nullptr;
  AdminPage* m_admin = nullptr;
};