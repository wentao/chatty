#include "room.h"

#include <QDebug>

#include "client.h"

Room::Room(const QString &name, QObject *parent)
    : QThread(parent), name_(name) {
  connect(this, &Room::finished, this, &Room::deleteLater);
}

Room::~Room() {}

void Room::run() {
  qDebug() << "Creating room" << name_;
  exec();
}

void Room::take(Client *client) {
  client->moveToThread(this);

  welcome(client);
}

void Room::welcome(Client *client) {}
