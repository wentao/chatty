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
  void send(QStringList msg);
  void registerProtocol(Protocol *protocol);
  void join(Room* room);

  void connected();
  void connectionFailed();

public slots:
  void socketReady(TcpHandle* socket);

  void disconnected();
  void readyRead(QByteArray data);

  void processPendingMessages();

  void transmit(QStringList msg);
  void protocolRegistration(Protocol *protocol);

  void enterRoom(Room* room);

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
