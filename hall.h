#ifndef HALL_H
#define HALL_H

#include "room.h"

#include <map>
#include <utility>

#include "protocol.h"

class Hall : public Room {
  Q_OBJECT
public:
  static Hall *Get() { return &hall_; }
  ~Hall() override;

signals:

public slots:

private:
  static Hall hall_;
  static std::map<QString, Room *> opens_;

  class Login : public Protocol {
  public:
    explicit Login(const std::map<QString, Client *> &users);
    ~Login() override;

    const QString &intro() const override;
    bool execute(const QString &input, QString *output) override;
    bool finished() override;

  private:
    static QString welcome_;

    const std::map<QString, Client *> &users_;
    QString name_;
  };

  explicit Hall(QObject *parent = 0);
  std::pair<Room *, bool> CreateRoom(const QString &name);

  std::map<QString, Client *> users_;
};

#endif // HALL_H
