#include "hall.h"

Hall Hall::hall_;

std::map<QString, Room*> Hall::opens_;

Hall::Hall(QObject* parent) : Room("", parent) {}

Hall::~Hall() {}

std::pair<Room*, bool> Hall::CreateRoom(const QString& name) {
  auto it = opens_.find(name);
  if (it == opens_.end()) {
    Room* room = new Room(name);
    opens_[name] = room;
    room->start();
    return std::make_pair(room, true);
  } else {
    return std::make_pair(it->second, false);
  }
}
