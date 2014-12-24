#include "hall.h"

Hall Hall::hall_;

std::map<QString, Room *> Hall::opens_;

Hall::Hall(QObject *parent) : Room("", parent) {}

Hall::~Hall() {}

std::pair<Room *, bool> Hall::CreateRoom(const QString &name) {
  auto it = opens_.find(name);
  if (it == opens_.end()) {
    Room *room = new Room(name);
    opens_[name] = room;
    room->start();
    return std::make_pair(room, true);
  } else {
    return std::make_pair(it->second, false);
  }
}

QString Hall::Login::welcome_("Welcome to chatty, please tell us your name:");

Hall::Login::Login(const std::map<QString, Client *> &users)
    : Protocol(), users_(users), name_("") {}

Hall::Login::~Login() {}

const QString &Hall::Login::intro() const { return welcome_; }

bool Hall::Login::execute(const QString &input, QString *output) {
  QString name = input.trimmed();
  if (users_.find(name) != users_.end()) {
    *output = "name already taken, try another";
    return false;
  } else if (name.length() == 0) {
    *output = "name can't be empty";
    return false;
  } else {
    *output = "";
    name_ = name;
    return true;
  }
}

bool Hall::Login::finished() { return name_.length() > 0; }
