#include "ProtocolCodec.h"
#include <QJsonDocument>

QByteArray ProtocolCodec::encode(const QJsonObject& obj) const {
  return QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
}

bool ProtocolCodec::tryDecodeLine(const QByteArray& line, QJsonObject& out) const {
  QJsonParseError err{};
  const auto doc = QJsonDocument::fromJson(line.trimmed(), &err);
  if (err.error != QJsonParseError::NoError) return false;
  if (!doc.isObject()) return false;
  out = doc.object();
  return true;
}