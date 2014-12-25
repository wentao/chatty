#include "room.h"

#include <QDebug>

#include "client.h"

Room::Room(const QString &name, QObject *parent)
  : QObject(parent), name_(name), looper_(parent) {
}

Room::~Room() {
  qDebug() << "Room" << name_ << "destroyed";
}

void Room::startLooper() {
  looper_.start();
  moveToThread(&looper_);
}

void Room::welcome(Client *client) {
}
