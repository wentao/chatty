#ifndef ROOM_H
#define ROOM_H

#include <vector>

#include <QThread>
#include <QString>

#include <client.h>

class Room : public QThread {
  Q_OBJECT
public:
  static Room* Hall() { return &hall_; }

  Room(const QString& name = "", QObject* parent = 0);
  ~Room() override;

  void accept(Client* client);

protected:
  void run() override;

signals:

public slots:

private:
  static Room hall_;
  static std::vector<Room*> rooms_;

  QString name_;
};

#endif // ROOM_H
