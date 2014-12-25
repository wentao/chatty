#ifndef ROOM_H
#define ROOM_H

#include <QObject>
#include <QString>
#include <QThread>

class Client;
class Hall;

class Room : public QObject {
  Q_OBJECT
public:
  ~Room() override;

  void startLooper();
  virtual void welcome(Client *client);

signals:

public slots:

protected:
  Room(const QString &name, QObject *parent = 0);

  QString name_;
  QThread looper_;

  friend class Hall;
};

#endif // ROOM_H
