#include "client.h"

#include <QThreadPool>

#include "room.h"
#include "hall.h"

Client::Client(QObject *parent)
  : QObject(parent), socket_(nullptr), room_(nullptr) {
  connect(this, &Client::join, this, &Client::enterRoom, Qt::QueuedConnection);
}

Client::~Client() {
  qDebug() << "Client" << socketDescriptor_ << "destroyed";
}

void Client::enterRoom(Room *room) {
  moveToThread(room->thread());
  room_ = room;
  if (room != nullptr) {
    qDebug() << "Client" << socketDescriptor_ << "joined room" << room->name();
  }
  room->welcome(this);
}

void Client::establishConnection(qintptr socketDescriptor) {
  TcpHandleCreator *creator = new TcpHandleCreator(socketDescriptor);

  connect(creator, &TcpHandleCreator::socketReady, this, &Client::socketReady);
  connect(creator, &TcpHandleCreator::socketFailed, this, &Client::connectionFailed);

  creator->execute();
  // QThreadPool::globalInstance()->start(creator);
}

void Client::socketReady(TcpHandle *socket) {
  socket_ = socket;

  socketDescriptor_ = socket->socketDescriptor();

  connect(socket_, &TcpHandle::receive, this, &Client::readyRead);
  connect(socket_, &TcpHandle::disconnected, this, &Client::disconnected);

  connect(this, &Client::send, &Client::transmit);
  connect(this, &Client::registerProtocol, &Client::protocolRegistration);

  qDebug() << "Client" << socketDescriptor_ << "connected";
  buffer_.clear();

  emit connected();
}

void Client::disconnected() {
  qDebug() << "Client" << socketDescriptor_ << "disconnected";
  socketDescriptor_ = -1;
  socket_->deleteLater();
}

void Client::readyRead(QString msg) {
  pendingMessages_.push_back(msg);
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
  for (int i = 0; i < msg.size(); ++i) {
    emit socket_->send(msg.at(i));
  }
}

void Client::protocolRegistration(Protocol *protocol) {
  protocols_.emplace(protocol);
  if (protocol->intro() != nullptr) {
    emit socket_->send(*protocol->intro());
  }
}
