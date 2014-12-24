#ifndef CLIENT_H
#define CLIENT_H

#include <deque>

#include <QByteArray>
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

signals:
  void send(QStringList msg);

public slots:
  void disconnected();
  void readyRead();

  void processPendingMessages();

  void transmit(QStringList msg);

private:
  QString name_;

  qintptr socketDescriptor_;
  QTcpSocket *socket_;

  QByteArray buffer_;
  std::deque<QString> pendingMessages_;

  Room *room_;
};

#endif // CLIENT_H
