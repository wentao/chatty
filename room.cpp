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
  client->registerProtocol(new Talk(this, client));
  QStringList userList;
  userList << "users list...";
  client->send(userList);
}

Talk::Talk(Room *room, Client *client) : room_(room), client_(client) {}

Talk::~Talk() {}

bool Talk::execute(const QString &input, QStringList *output) {
  *output << client_->name();
  output->last().append(": ").append(input);
  return false;
}
