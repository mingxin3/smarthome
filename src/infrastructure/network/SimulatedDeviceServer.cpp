#include "SimulatedDeviceServer.h"
#include "ProtocolCodec.h"
#include "infrastructure/persistence/repositories/DeviceRepository.h"

#include <QJsonObject>
#include <QJsonDocument>

SimulatedDeviceServer::SimulatedDeviceServer(ProtocolCodec& codec, DeviceRepository& repo, QObject* parent)
  : QObject(parent), m_codec(codec), m_repo(repo) {
  connect(&m_server, &QTcpServer::newConnection, this, &SimulatedDeviceServer::onNewConnection);
}

bool SimulatedDeviceServer::start(const QHostAddress& addr, quint16 port) {
  return m_server.listen(addr, port);
}

void SimulatedDeviceServer::onNewConnection() {
  while (auto* sock = m_server.nextPendingConnection()) {
    m_buffers.insert(sock, {});
    connect(sock, &QTcpSocket::readyRead, this, &SimulatedDeviceServer::onClientReadyRead);
    connect(sock, &QTcpSocket::disconnected, this, &SimulatedDeviceServer::onClientDisconnected);

    // On connect: push all states
    broadcastAllStates(sock);
  }
}

void SimulatedDeviceServer::onClientReadyRead() {
  auto* sock = qobject_cast<QTcpSocket*>(sender());
  if (!sock) return;

  QByteArray& buf = m_buffers[sock];
  buf += sock->readAll();

  while (true) {
    int idx = buf.indexOf('\n');
    if (idx < 0) break;
    const QByteArray line = buf.left(idx);
    buf.remove(0, idx + 1);

    QJsonObject msg;
    if (m_codec.tryDecodeLine(line, msg)) {
      handleMessage(sock, msg);
    }
  }
}

void SimulatedDeviceServer::onClientDisconnected() {
  auto* sock = qobject_cast<QTcpSocket*>(sender());
  if (!sock) return;
  m_buffers.remove(sock);
  sock->deleteLater();
}

void SimulatedDeviceServer::handleMessage(QTcpSocket* sock, const QJsonObject& msg) {
  const QString type = msg.value("type").toString();
  if (type == "set") {
    const int deviceId = msg.value("deviceId").toInt();
    const QJsonObject state = msg.value("state").toObject();

    // apply to DB
    m_repo.updateDeviceState(deviceId, state, true);

    // echo updated state
    QJsonObject out;
    out["type"] = "state";
    out["deviceId"] = deviceId;
    out["online"] = true;
    out["state"] = state;
    sock->write(m_codec.encode(out));
  } else if (type == "ping") {
    QJsonObject out;
    out["type"] = "pong";
    sock->write(m_codec.encode(out));
  } else if (type == "getAll") {
    broadcastAllStates(sock);
  }
}

void SimulatedDeviceServer::broadcastAllStates(QTcpSocket* sock) {
  const auto devices = m_repo.listDevices();
  for (const auto& d : devices) {
    QJsonObject out;
    out["type"] = "state";
    out["deviceId"] = d.id;
    out["online"] = d.online;
    out["state"] = d.state;
    sock->write(m_codec.encode(out));
  }
}