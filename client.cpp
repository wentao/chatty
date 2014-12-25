#include "client.h"

#include <QDebug>

#include "room.h"
#include "hall.h"

void TrimRight(QString &str) {
  while (str.size() > 0 && str.at(str.size() - 1).isSpace()) {
    str.chop(1);
  }
}

Client::Client(QObject *parent)
  : QObject(parent), socket_(nullptr), room_(nullptr) {}

Client::~Client() {
  qDebug() << "Client" << socketDescriptor_ << "destroyed";
}

bool Client::establishConnection(qintptr socketDescriptor) {
  socket_ = new QTcpSocket(this);
  if (!socket_->setSocketDescriptor(socketDescriptor)) {
    qDebug() << "Can't initialize the connection to client" << socketDescriptor;
    return false;
  }

  socketDescriptor_ = socketDescriptor;

  connect(socket_, &QTcpSocket::readyRead, this, &Client::readyRead);
  connect(socket_, &QTcpSocket::disconnected, this, &Client::disconnected);

  connect(this, &Client::send, &Client::transmit);
  connect(this, &Client::registerProtocol, &Client::protocolRegistration);

  qDebug() << "Client" << socketDescriptor << "connected";
  buffer_.clear();

  return true;
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
      pendingMessages_.emplace_back(buffer_);
      TrimRight(pendingMessages_.back()); // remove \r and other ending spaces
      buffer_.clear();
    } else {
      buffer_.append(ch);
    }
  }

  processPendingMessages();
}

void Client::processPendingMessages() {
  while (!pendingMessages_.empty()) {
    const QString &msg = pendingMessages_.front();
    if (!protocols_.empty()) {
      QString response;
      if (protocols_.top()->execute(msg, &response)) {
        protocols_.pop();
      }
      if (!response.isEmpty()) writeMessage(response);
    } else {
      qDebug() << "ignore message" << msg;
    }
    pendingMessages_.pop_front();
  }
}

void Client::transmit(QStringList msg) {
  if (socket_ != nullptr) {
    for (int i = 0; i < msg.size(); ++i) {
      writeMessage(msg.at(i));
    }
    socket_->flush();
  }
}

void Client::writeMessage(const QString &msg) {
  socket_->write("> ");
  socket_->write(msg.toUtf8());
  socket_->write("\r\n"); // for telnet line termination
}

void Client::protocolRegistration(Protocol *protocol) {
  protocols_.emplace(protocol);
  if (protocol->intro() != nullptr) {
    writeMessage(*protocol->intro());
  }
}
