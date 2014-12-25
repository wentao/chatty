#include "tcphandle.h"

#include <QDebug>

void TrimRight(QString &str) {
  while (str.size() > 0 && str.at(str.size() - 1).isSpace()) {
    str.chop(1);
  }
}

TcpHandle::TcpHandle(QObject *parent) : QTcpSocket(parent) {}

TcpHandle::~TcpHandle() {}

void TcpHandle::init() {
  connect(this, &TcpHandle::readyRead, this, &TcpHandle::truncRead);
  connect(this, &TcpHandle::send, this, &TcpHandle::transmit);
}

void TcpHandle::truncRead() {
  qDebug() << "reading...";
  char ch;
  while (read(&ch, 1) == 1) {
    if (ch == '\n') {
      QString msg(buffer_);
      TrimRight(msg); // remove \r and other ending spaces
      qDebug() << "receive" << msg;
      emit receive(msg);
      buffer_.clear();
    } else {
      buffer_.append(ch);
    }
  }
}

void TcpHandle::transmit(QString msg) {
  qDebug() << "send" << msg;
  write("> ");
  write(msg.toUtf8());
  write("\r\n"); // for telnet line termination
}

QThread TcpHandleCreator::socketThread_;

TcpHandleCreator::TcpHandleCreator(qintptr socketDescriptor)
  : socketDescriptor_(socketDescriptor) {
  setAutoDelete(true);
  connect(this, &TcpHandleCreator::create, this, &TcpHandleCreator::doCreation);
}

TcpHandleCreator::~TcpHandleCreator() {}

void TcpHandleCreator::execute() {
  if (!socketThread_.isRunning()) {
    socketThread_.start();
  }

  moveToThread(&socketThread_);
  emit create();
}

void TcpHandleCreator::doCreation() {
  run();
}

void TcpHandleCreator::run() {
  TcpHandle *socket = new TcpHandle;
  if (socket->setSocketDescriptor(socketDescriptor_)) {
    qDebug() << "Initialize the connection to client" << socketDescriptor_;
    socket->init();
    emit socketReady(socket);
  } else {
    qDebug() << "Can't initialize the connection to client" << socketDescriptor_;
    delete socket;
    emit socketFailed();
  }
}
