#ifndef TCPHANDLECREATOR_H
#define TCPHANDLECREATOR_H

#include <QObject>
#include <QRunnable>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QThread>

// The TCP socket wrapper that allows other threads to share
// a socket to send/receive TCP data packets.
class TcpHandle : public QTcpSocket {
  Q_OBJECT
public:
  explicit TcpHandle(QObject *parent = 0);
  ~TcpHandle() override;

  void init();

signals:
  // Signals when new message has been received and buffered.
  void receive(QByteArray data);
  // Signals a new message will be sent out soon.
  void send(QString msg);

  // Signals to shutdown the socket.
  void shutdown();

public slots:
  void transmit(QString msg);
  void truncRead();
  void terminate();

private:
  QByteArray buffer_;
};

// The helper class to instantiate the socket handle in a dedicated thread.
class TcpHandleCreator : public QObject, public QRunnable {
  Q_OBJECT
public:
  explicit TcpHandleCreator(qintptr socketDescriptor);
  ~TcpHandleCreator() override;

  void execute();

signals:
  void socketReady(TcpHandle *socket);
  void socketFailed();

  void create();

public slots:
  void doCreation();

protected:
  void run() override;

private:
  static QThread socketThread_;

  qintptr socketDescriptor_;
};


#endif // TCPHANDLECREATOR_H
