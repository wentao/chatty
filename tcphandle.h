#ifndef TCPHANDLECREATOR_H
#define TCPHANDLECREATOR_H

#include <QObject>
#include <QRunnable>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QThread>

class TcpHandle : public QTcpSocket {
  Q_OBJECT
public:
  explicit TcpHandle(QObject *parent = 0);
  ~TcpHandle() override;

  void init();

signals:
  void receive(QByteArray data);
  void send(QString msg);

  void shutdown();

public slots:
  void transmit(QString msg);
  void truncRead();
  void terminate();

private:
  QByteArray buffer_;
};

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
