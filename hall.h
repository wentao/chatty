#ifndef HALL_H
#define HALL_H

#include "room.h"

#include <map>
#include <utility>

class Hall : public Room {
  Q_OBJECT
public:
  static Hall* Get() { return &hall_; }
  ~Hall() override;

signals:

public slots:

private:
  static Hall hall_;
  static std::map<QString, Room*> opens_;

  explicit Hall(QObject* parent = 0);
  std::pair<Room*, bool> CreateRoom(const QString& name);
};

#endif // HALL_H
