#include "room.h"

#include <QDebug>

Room Room::hall_;

Room::Room(const QString& name, QObject* parent) : QThread(parent), name_(name) {
  connect(this, &Room::finished, this, &Room::deleteLater);
}

Room::~Room() {}

void Room::run() {
  qDebug() << "Creating room" << name_;
  exec();
}

void Room::accept(Client* client) {
  client->moveToThread(this);
}
