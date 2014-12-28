#ifndef HALL_H
#define HALL_H

#include <map>
#include <utility>

#include <qglobal.h>

#include "room.h"
#include "protocol.h"
#include "server.h"

class Login;
class Pin;
class HallAction;
class Join;
class Create;

// This is the server instance that manages connected clients
// who haven't joined any room yet.
class Hall : public Server {
  Q_OBJECT
public:
  static Hall *Get() { return &hall_; }
  ~Hall() override;

  void welcome(Client *client) override;

  // Set up the default rooms.
  void setupRooms() {
    CreateRoom("default").first->startLooper();
  }

signals:
  // Signals a new connection has been accepted by TCP socket server.
  void newConnection(qintptr socketDescriptor);

public slots:
  // Creates a new client to handle the new connection.
  void newClient(qintptr socketDescriptor);

  // Processes a client has quit the Hall.
  void quit(Client *client);

private:
  static Hall hall_;
  static std::map<QString, Room *> opens_;
  static std::pair<Room *, bool> CreateRoom(const QString &name, const QString &pin = "");

  explicit Hall(QObject *parent = 0);

  std::map<QString, Client *> users_;

  friend class Login;
  friend class Pin;
  friend class HallAction;
  friend class Join;
  friend class Create;
};

// The protocol that handles client's login behavior.
class Login : public Protocol {
public:
  Login(Hall *hall, Client *client);
  ~Login() override;

  const QString *intro() const override;
  bool execute(const QString &input, QStringList *output) override;
  bool finished() override;

private:
  static QString welcome_;

  Hall *hall_;
  Client *client_;
  QString name_;
};

// The protocol that enforce a client to enter pin code
// before joining a private room.
class Pin : public Protocol {
public:
  Pin(Room *room, Client *client);
  ~Pin() override;

  const QString *intro() const override;
  bool execute(const QString &input, QStringList *output) override;
  bool finished() override;

private:
  static QString msg_;

  Room *room_;
  Client *client_;
  bool pass_;
};

// The protocol that delegates all actions a client can use
// in the Hall.
class HallAction : public Protocol {
public:
  HallAction(Hall *hall, Client *client);
  ~HallAction() override;

  const QString *intro() const override;
  bool execute(const QString &input, QStringList *output) override;
  bool finished() override;

private:
  static QString actionList_;

  Hall *hall_;
  Client *client_;
};

// The protocol that progresses the client to join a room.
class Join : public Command {
public:
  Join(Hall *hall, Client *client);
  ~Join() override;

  using Command::execute;

protected:
  bool execute(QStringList *output) override;

private:
  Hall *hall_;
  Client *client_;
};

// The protocol that progresses the client to create a room.
class Create : public Command {
public:
  Create();
  ~Create() override;

  using Command::execute;

protected:
  bool execute(QStringList *output) override;
};

#endif // HALL_H
