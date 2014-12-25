#ifndef CLIENT_H
#define CLIENT_H

#include <deque>
#include <stack>
#include <memory>

#include <QByteArray>
#include <QDebug>
#include <QObject>
#include <QStringList>
#include <QTcpSocket>

#include <protocol.h>

class Room;

class Client : public QObject {
  Q_OBJECT
public:
  explicit Client(QObject *parent = 0);
  ~Client() override;

  bool establishConnection(qintptr socketDescriptor);

  const QString &name() const { return name_; }
  void setName(const QString &name) {
    name_ = name;
    qDebug() << "Client" << socketDescriptor_ << "logged in as" << name_;
  }

  void setRoom(Room *room = nullptr) { room_ = room; }

signals:
  void send(QStringList msg);
  void registerProtocol(Protocol *protocol);

public slots:
  void disconnected();
  void readyRead();

  void processPendingMessages();

  void transmit(QStringList msg);
  void protocolRegistration(Protocol *protocol);

private:
  void writeMessage(const QString &msg);

  QString name_;

  qintptr socketDescriptor_;
  QTcpSocket *socket_;

  QByteArray buffer_;
  std::deque<QString> pendingMessages_;

  Room *room_;
  std::stack<std::unique_ptr<Protocol>> protocols_;
};

#endif // CLIENT_H
