#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>

class ProtocolCodec;
class DeviceRepository;

class SimulatedDeviceServer : public QObject {
  Q_OBJECT
public:
  explicit SimulatedDeviceServer(ProtocolCodec& codec, DeviceRepository& repo, QObject* parent = nullptr);

  bool start(const QHostAddress& addr, quint16 port);

private slots:
  void onNewConnection();
  void onClientReadyRead();
  void onClientDisconnected();

private:
  void handleMessage(QTcpSocket* sock, const QJsonObject& msg);
  void broadcastAllStates(QTcpSocket* sock);

  ProtocolCodec& m_codec;
  DeviceRepository& m_repo;

  QTcpServer m_server;
  QHash<QTcpSocket*, QByteArray> m_buffers;
};