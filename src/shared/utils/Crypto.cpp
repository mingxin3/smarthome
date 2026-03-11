#include "Crypto.h"

#include <QCryptographicHash>

QString Crypto::hashPassword(const QString& plain) {
  // 教学项目简化：SHA256(plain). 真实项目应加 salt/迭代等。
  const QByteArray h = QCryptographicHash::hash(plain.toUtf8(), QCryptographicHash::Sha256);
  return QString::fromLatin1(h.toHex());
}