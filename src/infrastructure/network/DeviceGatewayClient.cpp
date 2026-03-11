#include "DeviceGatewayClient.h"
#include "ProtocolCodec.h"

#include <QDebug>

DeviceGatewayClient::DeviceGatewayClient(ProtocolCodec& codec, QObject* parent)
  : QObject(parent), m_codec(codec) {

  connect(&m_sock, &QTcpSocket::readyRead, this, &DeviceGatewayClient::onReadyRead);
  connect(&m_sock, &QTcpSocket::connected, this, &DeviceGatewayClient::connected);
  connect(&m_sock, &QTcpSocket::disconnected, this, &DeviceGatewayClient::onDisconnected);
}

void DeviceGatewayClient::connectToHost(const QString& host, quint16 port) {
  if (m_sock.state() != QAbstractSocket::UnconnectedState) m_sock.abort();
  m_sock.connectToHost(host, port);
}

bool DeviceGatewayClient::isConnected() const {
  return m_sock.state() == QAbstractSocket::ConnectedState;
}

void DeviceGatewayClient::send(const QJsonObject& obj) {
  if (!isConnected()) return;
  m_sock.write(m_codec.encode(obj));
}

void DeviceGatewayClient::onReadyRead() {
  m_buffer += m_sock.readAll();
  while (true) {
    int idx = m_buffer.indexOf('\n');
    if (idx < 0) break;
    const QByteArray line = m_buffer.left(idx);
    m_buffer.remove(0, idx + 1);

    QJsonObject msg;
    if (m_codec.tryDecodeLine(line, msg)) {
      emit messageReceived(msg);
    }
  }
}

void DeviceGatewayClient::onDisconnected() {
  emit disconnected();
}