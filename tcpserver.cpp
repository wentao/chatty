#include "tcpserver.h"

#include "client.h"
#include "hall.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent) {}

TcpServer::~TcpServer() {}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
  emit Hall::Get()->newConnection(socketDescriptor);
}
