#ifndef ROOM_H
#define ROOM_H

#include <QThread>
#include <QString>

class Client;
class Hall;

class Room : public QThread {
  Q_OBJECT
public:
  ~Room() override;

protected:
  void run() override;

signals:

public slots:

protected:
  Room(const QString& name, QObject* parent = 0);

  QString name_;

  friend class Hall;
};

#endif // ROOM_H
