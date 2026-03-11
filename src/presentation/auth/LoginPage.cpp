#include "LoginPage.h"
#include "ui_LoginPage.h"

#include "services/AuthService.h"

#include <QMessageBox>

LoginPage::LoginPage(AuthService& auth, QWidget *parent)
  : QWidget(parent), ui(new Ui::LoginPage), m_auth(auth) {
  ui->setupUi(this);
  connect(ui->btnLogin, &QPushButton::clicked, this, &LoginPage::doLogin);
}

LoginPage::~LoginPage() { delete ui; }

void LoginPage::doLogin() {
  const QString u = ui->edtUser->text().trimmed();
  const QString p = ui->edtPass->text();
  if (m_auth.login(u, p)) {
    emit loggedIn(u);
  } else {
    QMessageBox::warning(this, "登录失败", "用户名或密码错误。");
  }
}