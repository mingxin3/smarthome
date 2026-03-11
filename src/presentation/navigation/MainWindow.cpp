#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "presentation/auth/LoginPage.h"
#include "presentation/home/HomePage.h"
#include "presentation/devices/DevicesPage.h"
#include "presentation/scenes/ScenesPage.h"
#include "presentation/history/HistoryPage.h"
#include "presentation/alarms/AlarmsPage.h"
#include "presentation/admin/AdminPage.h"

#include "services/AlarmService.h"
#include "infrastructure/settings/SettingsManager.h"

#include <QMessageBox>

MainWindow::MainWindow(AppContext& ctx, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_ctx(ctx) {
    ui->setupUi(this);

    // 1) 一次性创建所有页面并加入 stackedWidget
    m_login   = new LoginPage(*m_ctx.authService, this);
    m_home    = new HomePage(m_ctx, this);
    m_devices = new DevicesPage(m_ctx, this);
    m_scenes  = new ScenesPage(m_ctx, this);
    m_history = new HistoryPage(m_ctx, this);
    m_alarms  = new AlarmsPage(m_ctx, this);
    m_admin   = new AdminPage(m_ctx, this);

    ui->stackedWidget->addWidget(m_login);
    ui->stackedWidget->addWidget(m_home);
    ui->stackedWidget->addWidget(m_devices);
    ui->stackedWidget->addWidget(m_scenes);
    ui->stackedWidget->addWidget(m_history);
    ui->stackedWidget->addWidget(m_alarms);
    ui->stackedWidget->addWidget(m_admin);

    // 2) 导航按钮只切换页面（登录后才允许）
    connect(ui->btnHome, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentWidget(m_home); });
    connect(ui->btnDevices, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentWidget(m_devices); });
    connect(ui->btnScenes, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentWidget(m_scenes); });
    connect(ui->btnHistory, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentWidget(m_history); });
    connect(ui->btnAlarms, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentWidget(m_alarms); });
    connect(ui->btnAdmin, &QPushButton::clicked, this, [this]{ ui->stackedWidget->setCurrentWidget(m_admin); });
    connect(ui->btnLogout, &QPushButton::clicked, this, &MainWindow::logout);

    // 3) 登录信号只连一次
    connect(m_login, &LoginPage::loggedIn, this, &MainWindow::onLoggedIn);

    // 4) 告警弹窗
    connect(m_ctx.alarmService.get(), &AlarmService::alarmRaised, this, [this](const QString& msg){
        if (m_ctx.settings && m_ctx.settings->get("alarm/popup_enabled", true).toBool()) {
            QMessageBox::warning(this, QString::fromUtf8("异常报警"), msg);
        }
    });

    // 初始：显示登录页，禁用导航
    ui->stackedWidget->setCurrentWidget(m_login);
    ui->navFrame->setEnabled(false);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onLoggedIn(QString username) {
    m_currentUser = username;

    // 登录成功：启用导航并切到首页
    ui->navFrame->setEnabled(true);
    ui->stackedWidget->setCurrentWidget(m_home);
}

void MainWindow::logout() {
    m_currentUser.clear();

    // 退出：禁用导航并回登录页
    ui->navFrame->setEnabled(false);
    ui->stackedWidget->setCurrentWidget(m_login);
}
