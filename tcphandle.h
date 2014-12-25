#ifndef TCPHANDLECREATOR_H
#define TCPHANDLECREATOR_H

#include <QObject>
#include <QRunnable>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>

class TcpHandle : public QTcpSocket {
  Q_OBJECT
public:
  explicit TcpHandle(QObject *parent = 0);
  ~TcpHandle() override;

  void init();

signals:
  void receive(QString msg);
  void send(QString msg);

public slots:
  void transmit(QString msg);
  void truncRead();

private:
  QByteArray buffer_;
};

class TcpHandleCreator : public QObject, public QRunnable {
  Q_OBJECT
public:
  explicit TcpHandleCreator(qintptr socketDescriptor);
  ~TcpHandleCreator() override;

  void execute() { run(); }

signals:
  void socketReady(TcpHandle *socket);
  void socketFailed();

protected:
  void run() override;

private:
  qintptr socketDescriptor_;
};


#endif // TCPHANDLECREATOR_H
