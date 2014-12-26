#ifndef ROOM_H
#define ROOM_H

#include <map>

#include <QObject>
#include <QString>
#include <QThread>

#include <protocol.h>

class Client;
class Hall;

class Talk;
class Whisper;

class Room : public QObject {
  Q_OBJECT
public:
  ~Room() override;

  void startLooper();
  virtual void welcome(Client *client);

  const QString &name() const { return name_; }

signals:
  void join(Client* client);
  void leave(Client* client);

public slots:
  void joined(Client* client);
  void left(Client* client);

protected:
  Room(const QString &name, QObject *parent = 0);

  QStringList userNameList(Client *client = 0);
  void broadcast(QStringList msg, Client *except = 0);

  QString name_;
  QThread looper_;

  std::map<QString, Client *> users_;

  friend class Hall;
  friend class Talk;
  friend class Whisper;
};

class Talk : public Protocol {
public:
  Talk(Room *room, Client *client);
  ~Talk() override;

  bool execute(const QString &input, QStringList *output) override;

private:
  Room *room_;
  Client *client_;
};

class Whisper : public Command {
public:
  Whisper(Room *room, Client *client);
  ~Whisper() override;

  using Command::execute;

protected:
  bool execute(QStringList *output) override;

private:
  Room *room_;
  Client *client_;
};

#endif // ROOM_H
