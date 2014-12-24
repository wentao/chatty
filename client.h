#ifndef CLIENT_H
#define CLIENT_H

#include <deque>

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QTcpSocket>

class Room;

class Client : public QObject {
  Q_OBJECT
public:
  explicit Client(QObject* parent = 0);
  ~Client() override;

  bool establishConnection(qintptr socketDescriptor);
  void join(Room* room);
  void leave();

signals:
  void send(QString msg);

public slots:
  void disconnected();
  void readyRead();

  void processPendingMessages();

  void transmit(QString msg);

private:
  QString name_;

  qintptr socketDescriptor_;
  QTcpSocket* socket_;

  QByteArray buffer_;
  std::deque<QString> pendingMessages_;

  Room* room_;
};

#endif // CLIENT_H
