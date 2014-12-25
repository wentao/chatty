#include "tcphandle.h"

#include <QDebug>

TcpHandle::TcpHandle(QObject *parent) : QTcpSocket(parent) {}

TcpHandle::~TcpHandle() {
  qDebug() << "Socket destroyed";
}

void TcpHandle::init() {
  connect(this, &TcpHandle::readyRead, this, &TcpHandle::truncRead);
  connect(this, &TcpHandle::send, this, &TcpHandle::transmit);
  connect(this, &TcpHandle::shutdown, this, &TcpHandle::terminate);
}

constexpr int kReadBufferSize = 8192;

void TcpHandle::truncRead() {
  // Avoid the case when one socket has large amount to read, which might
  // block other sockets from reading/writing.
  if (isOpen()) {
    receive(read(kReadBufferSize));
  }
}

void TcpHandle::transmit(QString msg) {
  if (isOpen()) {
    write("> ");
    // TODO(wentao): figure out a better way to handle the case when
    // msg is too large.
    write(msg.toUtf8());
    write("\r\n"); // for telnet line termination
  }
}

void TcpHandle::terminate() {
  close();
}

QThread TcpHandleCreator::socketThread_;

TcpHandleCreator::TcpHandleCreator(qintptr socketDescriptor)
  : socketDescriptor_(socketDescriptor) {
  setAutoDelete(true);
  connect(this, &TcpHandleCreator::create, this, &TcpHandleCreator::doCreation);
}

TcpHandleCreator::~TcpHandleCreator() {
  qDebug() << "Socket creator destroyed";
}

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

  // TODO(wentao): remove this if we are using this class in QThreadPool
  deleteLater();
}
