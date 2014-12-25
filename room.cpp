#include "room.h"

#include <QDebug>

#include "client.h"

Room::Room(const QString &name, QObject *parent)
  : QObject(parent), name_(name), looper_(parent) {
  connect(this, &Room::join, this, &Room::joined);
  connect(this, &Room::leave, this, &Room::left);
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
  emit join(client);
}

void Room::joined(Client *client) {
  users_.insert(client);
  emit client->send(userNameList(client));

  QString msg(client->name());
  msg.append(" has joined.");
  broadcast(QStringList(msg), client);
}

void Room::left(Client *client) {
  users_.erase(client);
  emit client->leave(this);

  QString msg(client->name());
  msg.append(" has left.");
  broadcast(QStringList(msg), client);
}

QStringList Room::userNameList(Client* client) {
  QStringList list;
  for (Client *user : users_) {
    list << " * ";
    list.last().append(user->name());
    if (user == client) {
      list.last().append(" (** this is you)");
    }
  }
  return list;
}

void Room::broadcast(QStringList msg, Client *except) {
  for (Client *user : users_) {
    if (user != except) {
      emit user->send(msg);
    }
  }
}

const char *kActionLeave = "/leave";

Talk::Talk(Room *room, Client *client) : room_(room), client_(client) {}

Talk::~Talk() {}

bool Talk::execute(const QString &input, QStringList *output) {
  QString head = Command::ParseHead(input);
  if (head == kActionLeave) {
    emit room_->leave(client_);
    return true;
  } else {
    QString msg = client_->name();
    msg.append(": ").append(input);
    room_->broadcast(QStringList(msg));
    return false;
  }
}
