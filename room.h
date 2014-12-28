#ifndef ROOM_H
#define ROOM_H

#include <map>

#include <QObject>
#include <QString>
#include <QThread>

#include "protocol.h"
#include "server.h"

class Client;
class Hall;

class Talk;
class Whisper;

// The server instance that manages connected clients in a chat room.
class Room : public Server {
  Q_OBJECT
public:
  ~Room() override;

  void welcome(Client *client) override;

  const QString &name() const { return name_; }
  const QString &pin() const { return pin_; }

signals:
  // Signals a client wants to join the room.
  void join(Client* client);

public slots:
  // Processes when a client has joined the room.
  void joined(Client* client);
  // Processes when a client has left the room.
  void left(Client* client);

protected:
  Room(const QString &name, const QString &pin, QObject *parent = 0);

  QStringList userNameList(Client *client = 0);
  void broadcast(QStringList msg, Client *except = 0);

  QString name_;
  QString pin_;

  std::map<QString, Client *> users_;

  friend class Hall;
  friend class Talk;
  friend class Whisper;
};

// The main protocol to handle client's talk action or delegate
// other actions to dedicated protocols.
class Talk : public Protocol {
public:
  Talk(Room *room, Client *client);
  ~Talk() override;

  bool execute(const QString &input, QStringList *output) override;

private:
  Room *room_;
  Client *client_;
};

// The protocol that progresses a client to send a private message to
// another client.
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
