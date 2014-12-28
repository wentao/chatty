#ifndef CLIENT_H
#define CLIENT_H

#include <deque>
#include <stack>
#include <memory>

#include <QByteArray>
#include <QDebug>
#include <QObject>
#include <QStringList>

#include "protocol.h"
#include "tcphandle.h"

class Room;

// This class maintains the TCP connection and data/states of a client.
class Client : public QObject {
  Q_OBJECT
public:
  explicit Client(QObject *parent = 0);
  ~Client() override;

  void establishConnection(qintptr socketDescriptor);

  const QString &name() const { return name_; }
  void setName(const QString &name) {
    name_ = name;
    qDebug() << "Client" << socketDescriptor_ << "logged in as" << name_;
  }

signals:
  // Send a message to the client.
  // The strings in the QStringList will be joined with '\r\n'
  void send(QStringList msg);

  // Register a new potocol on top of the existing one. Use it
  // to drive the interaction with the client.
  void registerProtocol(Protocol *protocol);

  // Signal to join a room.
  void join(Room *room);
  // Signal to leave a room.
  void leave(Room *room);

  // Actively close the connection from server.
  void closeConnection();

  // Notifies the connection has been successfully established.
  void connected();
  // Notifies the connection failed to establish.
  void connectionFailed();

public slots:
  // Processes the socket handle when it's ready to use.
  void socketReady(TcpHandle *socket);

  // Processes when socket is disconnected from the client.
  void disconnectedFromClient();

  // Processes when socket is disconnected from the server.
  void disconnectedFromServer();

  // Processes when socket input data are buffered for read.
  void readyRead(QByteArray data);

  // Processes messages received from the client.
  void processPendingMessages();

  // Transmits the message to the client via socket.
  void transmit(QStringList msg);

  // Performs protocol registration.
  void protocolRegistration(Protocol *protocol);

  void enterRoom(Room *room);
  void leaveRoom(Room *room);

private:
  QString name_;

  qintptr socketDescriptor_;
  TcpHandle *socket_;

  QByteArray buffer_;
  std::deque<QString> pendingMessages_;

  Room *room_;
  std::stack<std::unique_ptr<Protocol>> protocols_;
};

#endif // CLIENT_H
