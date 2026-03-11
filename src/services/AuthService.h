#pragma once
#include <QString>

class UserRepository;

class AuthService {
public:
  explicit AuthService(UserRepository& repo);

  bool login(const QString& username, const QString& plainPassword);

private:
  UserRepository& m_repo;
};