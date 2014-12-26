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
  users_[client->name()] = client;
  emit client->send(userNameList(client));

  QString msg(client->name());
  msg.append(" has joined.");
  broadcast(QStringList(msg), client);
}

void Room::left(Client *client) {
  users_.erase(client->name());

  QString msg(client->name());
  msg.append(" has left.");
  broadcast(QStringList(msg), client);
}

QStringList Room::userNameList(Client* client) {
  QStringList list;
  for (auto& it : users_) {
    Client *user = it.second;
    list << " * ";
    list.last().append(user->name());
    if (user == client) {
      list.last().append(" (** this is you)");
    }
  }
  return list;
}

void Room::broadcast(QStringList msg, Client *except) {
  for (auto& it : users_) {
    Client *user = it.second;
    if (user != except) {
      emit user->send(msg);
    }
  }
}

const char *kActionLeave = "/leave";
const char *kActionWho = "/who";
const char *kActionWhisper = "/whisper";

Talk::Talk(Room *room, Client *client) : room_(room), client_(client) {}

Talk::~Talk() {}

bool Talk::execute(const QString &input, QStringList *output) {
  QString head = Command::ParseHead(input);
  if (head == kActionLeave) {
    emit room_->leave(client_);
    emit client_->leave(room_);
    return true;
  } else if (head == kActionWhisper) {
    Whisper* w = new Whisper(room_, client_);
    if (!w->execute(input, output)) {
      client_->registerProtocol(w);
    } else {
      delete w;
    }
  } else if (head == kActionWho) {
    emit client_->send(room_->userNameList(client_));
  } else {
    QString msg = client_->name();
    msg.append(": ").append(input);
    room_->broadcast(QStringList(msg));
  }
  return false;
}

Whisper::Whisper(Room *room, Client *client)
  : Command(), room_(room), client_(client) {
  AddArgument("name", "the name of the person to whisper");
  AddArgument("msg", "the message to send");
}

Whisper::~Whisper() {
  qDebug() << "Whisper protocol destroyed";
}

bool Whisper::execute(QStringList *output) {
  auto it = room_->users_.find(args_[0].value);
  if (it == room_->users_.end()) {
    *output << "User ";
    output->last().append(args_[0].value).append(" is not in the room.");
    index_ = 0;
    return false;
  } else {
    QStringList msg;
    msg << "From ";
    msg.last().append(client_->name()).append(": ").append(args_[1].value);
    emit it->second->send(msg);

    *output << "To ";
    output->last().append(args_[0].value).append(": ").append(args_[1].value);
    return true;
  }
}
