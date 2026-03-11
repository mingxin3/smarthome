#include "AuthService.h"
#include "infrastructure/persistence/repositories/UserRepository.h"
#include "shared/utils/Crypto.h"

AuthService::AuthService(UserRepository& repo) : m_repo(repo) {}

bool AuthService::login(const QString& username, const QString& plainPassword) {
  return m_repo.validateLogin(username, Crypto::hashPassword(plainPassword));
}