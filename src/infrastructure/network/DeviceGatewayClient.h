#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class ProtocolCodec;

class DeviceGatewayClient : public QObject {
  Q_OBJECT
public:
  explicit DeviceGatewayClient(ProtocolCodec& codec, QObject* parent = nullptr);

  void connectToHost(const QString& host, quint16 port);
  bool isConnected() const;

  void send(const QJsonObject& obj);

signals:
  void connected();
  void disconnected();
  void messageReceived(QJsonObject msg);

private slots:
  void onReadyRead();
  void onDisconnected();

private:
  ProtocolCodec& m_codec;
  QTcpSocket m_sock;
  QByteArray m_buffer;
};