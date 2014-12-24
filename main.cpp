#include <QCoreApplication>
#include <QTcpServer>

#include <room.h>
#include <tcpserver.h>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  Room::Hall()->start();

  TcpServer server;
  if (!server.listen(QHostAddress::LocalHost, 8237)) {
    qDebug() << "unable to listen TCP message, quit";
    return -1;
  }

  return a.exec();
}
