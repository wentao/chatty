#ifndef HALL_H
#define HALL_H

#include "room.h"

#include <map>
#include <utility>

#include <qglobal.h>

#include "protocol.h"

class Login;
class HallAction;
class Join;

class Hall : public Room {
  Q_OBJECT
public:
  static Hall *Get() { return &hall_; }
  ~Hall() override;

  void welcome(Client *client) override;

  void setupRooms() {
    CreateRoom("1").first->startLooper();
    CreateRoom("2").first->startLooper();
  }

signals:
  void newConnection(qintptr socketDescriptor);

public slots:
  void newClient(qintptr socketDescriptor);
  void quit(Client *client);

private:
  static Hall hall_;
  static std::map<QString, Room *> opens_;

  explicit Hall(QObject *parent = 0);
  std::pair<Room *, bool> CreateRoom(const QString &name);

  std::map<QString, Client *> users_;

  friend class Login;
  friend class HallAction;
  friend class Join;
};

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

#endif // HALL_H
