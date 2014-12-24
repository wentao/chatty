#ifndef CLIENT_H
#define CLIENT_H

#include <deque>

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QTcpSocket>

class Client : public QObject {
  Q_OBJECT
public:
  explicit Client(QObject* parent = 0);
  ~Client() override;

  bool establishConnection(qintptr socketDescriptor);

signals:

public slots:
  void disconnected();
  void readyRead();

  void processPendingMessages();

private:
  QString name_;

  qintptr socketDescriptor_;
  QTcpSocket* socket_;

  QByteArray buffer_;
  std::deque<QString> pendingMessages_;
};

#endif // CLIENT_H
