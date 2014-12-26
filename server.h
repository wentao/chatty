#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QThread>

class Client;

class Server : public QObject {
  Q_OBJECT
public:
  explicit Server(QObject *parent = 0);
  ~Server() override;

  void startLooper();
  virtual void welcome(Client *client) = 0;

signals:
  void leave(Client* client);

public slots:

private:
  QThread looper_;
};

#endif // SERVER_H
