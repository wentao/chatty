#include "client.h"

#include "room.h"
#include "hall.h"

Client::Client(QObject *parent)
  : QObject(parent), socket_(nullptr), room_(nullptr) {
  connect(this, &Client::join, this, &Client::enterRoom, Qt::QueuedConnection);
  connect(this, &Client::leave, this, &Client::leaveRoom, Qt::QueuedConnection);
}

Client::~Client() {
  qDebug() << "Client destroyed";
}

void Client::enterRoom(Room *room) {
  moveToThread(room->thread());
  room_ = room;
  if (room != nullptr) {
    qDebug() << "Client" << socketDescriptor_ << "joined room" << room->name();
  }
  room->welcome(this);
}

void Client::leaveRoom(Room *room) {
  room_ = nullptr;
  moveToThread(Hall::Get()->thread());

  QString msg("You left the room: ");
  msg.append(room->name());
  emit send(QStringList(msg));

  if (room != nullptr) {
    qDebug() << "Client" << socketDescriptor_ << "left room" << room->name();
  }
}

void Client::establishConnection(qintptr socketDescriptor) {
  TcpHandleCreator *creator = new TcpHandleCreator(socketDescriptor);

  connect(creator, &TcpHandleCreator::socketReady, this, &Client::socketReady);
  connect(creator, &TcpHandleCreator::socketFailed, this, &Client::connectionFailed);

  creator->execute();
}

void Client::socketReady(TcpHandle *socket) {
  socket_ = socket;
  socketDescriptor_ = socket->socketDescriptor();
  connect(socket_, &TcpHandle::receive, this, &Client::readyRead);
  connect(socket_, &TcpHandle::disconnected, this, &Client::disconnectedFromClient);
  qDebug() << "Client" << socketDescriptor_ << "connected";
  buffer_.clear();

  connect(this, &Client::send, &Client::transmit);
  connect(this, &Client::registerProtocol, &Client::protocolRegistration);
  connect(this, &Client::closeConnection, &Client::disconnectedFromServer);
  emit connected();
}

#define ASSERT_SOCKET_VALID \
  if (socket_ == nullptr) { \
  qDebug() << "Invalid state in client" << socketDescriptor_ << __LINE__; \
  return; \
  }

void Client::disconnectedFromClient() {
  qDebug() << "Client" << socketDescriptor_ << "disconnected";
  socketDescriptor_ = -1;

  if (room_ != nullptr) {
    emit room_->leave(this);
  }
  emit Hall::Get()->leave(this);

  ASSERT_SOCKET_VALID
  socket_->deleteLater();
  socket_ = nullptr;
}

void Client::disconnectedFromServer() {
  ASSERT_SOCKET_VALID
  socket_->shutdown();
}

inline void TrimRight(QString &str) {
  while (str.size() > 0 && str.at(str.size() - 1).isSpace()) {
    str.chop(1);
  }
}

void Client::readyRead(QByteArray data) {
  buffer_.append(data);

  int from = 0, to = -1;
  while (from < buffer_.size() &&
         (to = buffer_.indexOf('\n', from)) > -1) {
    pendingMessages_.emplace_back(buffer_.mid(from, to - from));
    TrimRight(pendingMessages_.back());
    from = to + 1;
  }
  buffer_ = buffer_.right(buffer_.size() - from);

  processPendingMessages();
}

void Client::processPendingMessages() {
  while (!pendingMessages_.empty()) {
    const QString &msg = pendingMessages_.front();
    if (!protocols_.empty()) {
      QStringList response;
      if (protocols_.top()->execute(msg, &response)) {
        protocols_.pop();
      }
      if (!response.isEmpty()) transmit(response);
    } else {
      qDebug() << "ignore message" << msg;
    }
    pendingMessages_.pop_front();
  }
}

void Client::transmit(QStringList msg) {
  ASSERT_SOCKET_VALID
  for (int i = 0; i < msg.size(); ++i) {
    emit socket_->send(msg.at(i));
  }
}

void Client::protocolRegistration(Protocol *protocol) {
  ASSERT_SOCKET_VALID
  protocols_.emplace(protocol);
  if (protocol->intro() != nullptr) {
    emit socket_->send(*protocol->intro());
  }
}
