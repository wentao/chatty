#include "tcpserver.h"

#include "client.h"
#include "hall.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent) {}

TcpServer::~TcpServer() {}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
  Client *client = new Client;
  if (client->establishConnection(socketDescriptor)) {
    client->join(Hall::Get());
  }
}
