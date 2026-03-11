#include "App.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setApplicationName("SmartHomeMonitorQt6");
  a.setOrganizationName("SchoolProject");

  App app;
  if (!app.init()) {
    QMessageBox::critical(nullptr, "启动失败", "初始化失败（数据库/资源/端口占用等）。");
    return 1;
  }

  QWidget* w = app.createMainWindow();
  w->show();
  return a.exec();
}