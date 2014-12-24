#include "client.h"

#include <QDebug>

#include "room.h"
#include "hall.h"

void TrimRight(QString& str) {
  while (str.size() > 0 && str.at(str.size() - 1).isSpace()) str.chop(1);
}

Client::Client(QObject *parent)
  : QObject(parent), socket_(nullptr), room_(nullptr) {
}

Client::~Client() {
}

bool Client::establishConnection(qintptr socketDescriptor) {
  socket_ = new QTcpSocket(this);
  if(!socket_->setSocketDescriptor(socketDescriptor)) {
    qDebug() << "Can't initialize the connection to client" << socketDescriptor;
    return false;
  }

  socketDescriptor_ = socketDescriptor;

  connect(socket_, &QTcpSocket::readyRead, this, &Client::readyRead);
  connect(socket_, &QTcpSocket::disconnected, this, &Client::disconnected);
  connect(this, &Client::send, this, &Client::transmit);

  qDebug() << "Client" << socketDescriptor << "connected";
  buffer_.clear();

  return true;
}

void Client::join(Room* room) {
  if (room_ != nullptr) {
    // TODO(wentao): clean up old room
  }
  moveToThread(room);
  room_ = room;
}

void Client::leave() {
  if (room_ == nullptr && room_ == Hall::Get()) return;
  join(Hall::Get());
}

void Client::disconnected() {
  qDebug() << "Client" << socketDescriptor_ << "disconnected";
  socketDescriptor_ = -1;
  socket_->deleteLater();
}

void Client::readyRead() {
  char ch;
  while (socket_->read(&ch, 1) == 1) {
    if (ch == '\n') {
      QString msg(buffer_);
      TrimRight(msg); // remove \r and other ending spaces
      pendingMessages_.push_back(msg);
      buffer_.clear();
    } else {
      buffer_.append(ch);
    }
  }

  processPendingMessages();
}

void Client::processPendingMessages() {
  while (!pendingMessages_.empty()) {
    const QString& msg = pendingMessages_.front();
    qDebug() << "... processing" << msg << msg.size();
    pendingMessages_.pop_front();
  }
}

void Client::transmit(QString msg) {
  if (socket_ != nullptr) {
    socket_->write(msg.toUtf8());
    socket_->flush();
  }
}
