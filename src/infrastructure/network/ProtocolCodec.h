#pragma once
#include <QByteArray>
#include <QJsonObject>

class ProtocolCodec {
public:
  QByteArray encode(const QJsonObject& obj) const;
  bool tryDecodeLine(const QByteArray& line, QJsonObject& out) const;
};