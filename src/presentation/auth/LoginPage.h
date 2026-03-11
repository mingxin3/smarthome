#pragma once

#include <QWidget>

class AuthService;

QT_BEGIN_NAMESPACE
namespace Ui { class LoginPage; }
QT_END_NAMESPACE

class LoginPage : public QWidget {
  Q_OBJECT

public:
  explicit LoginPage(AuthService& auth, QWidget *parent = nullptr);
  ~LoginPage() override;

signals:
  void loggedIn(QString username);

private slots:
  void doLogin();

private:
  Ui::LoginPage *ui;
  AuthService& m_auth;
};