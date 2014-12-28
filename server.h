#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QThread>

class Client;

// A server instance that runs its own thread (looper).
class Server : public QObject {
  Q_OBJECT
public:
  explicit Server(QObject *parent = 0);
  ~Server() override;

  void startLooper();

  // Processes when a client has been aceepted by this server.
  virtual void welcome(Client *client) = 0;

signals:
  void leave(Client* client);

public slots:

private:
  QThread looper_;
};

#endif // SERVER_H
