#include <QCoreApplication>
#include <QTcpServer>

#include <hall.h>
#include <tcpserver.h>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  Hall::Get()->startLooper();
  Hall::Get()->setupRooms();

  TcpServer server;
  if (!server.listen(QHostAddress::AnyIPv4, 8237)) {
    qDebug() << "unable to listen TCP message, quit";
    return -1;
  }

  return a.exec();
}
