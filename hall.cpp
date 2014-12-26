#include "hall.h"

#include "client.h"

#include <QStringList>
#include <QRegExp>

Hall Hall::hall_;

std::map<QString, Room *> Hall::opens_;

Hall::Hall(QObject *parent) : Room("", "", parent) {
  connect(this, &Hall::newConnection, &Hall::newClient);

  disconnect(this, &Room::leave, this, &Room::left);
  connect(this, &Room::leave, this, &Hall::quit);
}

Hall::~Hall() {}

std::pair<Room *, bool> Hall::CreateRoom(const QString &name, const QString &pin) {
  auto it = opens_.find(name);
  if (it == opens_.end()) {
    Room *room = new Room(name, pin);
    opens_[name] = room;
    return std::make_pair(room, true);
  } else {
    return std::make_pair(it->second, false);
  }
}

void Hall::welcome(Client *client) {
  client->registerProtocol(new HallAction(this, client));
  client->registerProtocol(new Login(this, client));
}

void Hall::newClient(qintptr socketDescriptor) {
  Client *client = new Client;
  connect(client, &Client::connected, [client, this]() {
    welcome(client);
  });
  connect(client, &Client::connectionFailed, [client]() {
    client->deleteLater();
  });
  client->establishConnection(socketDescriptor);
}

void Hall::quit(Client *client) {
  users_.erase(client->name());
  qDebug() << "Client" << client->name() << "quit.";
  client->deleteLater();
}

QString Login::welcome_("Welcome to chatty, please tell us your name:");

const QRegExp kWhitespaces("\\s+");

Login::Login(Hall *hall, Client *client)
  : Protocol(), hall_(hall), client_(client), name_("") {}

Login::~Login() {
  qDebug() << "Login protocol destroyed";
}

const QString *Login::intro() const { return &welcome_; }

bool Login::execute(const QString &input, QStringList* output) {
  QString name = input.trimmed();
  QString msg;
  if (hall_->users_.find(name) != hall_->users_.end()) {
    msg.append("Sorry, ").append(name).append(" already taken, try another?");
    *output << msg;
    return false;
  } else if (name.length() == 0) {
    msg.append("Name can't be empty! Try another?");
    *output << msg;
    return false;
  } else if (name.indexOf(kWhitespaces) > -1) {
    *output << "Don't accept string with whitespaces in the middle! Try another";
    return false;
  } else {
    msg.append("Welcome ").append(name).append("!");
    *output << msg;
    name_ = name;
    hall_->users_[name] = client_;
    client_->setName(name);
    return true;
  }
}

bool Login::finished() { return name_.length() > 0; }

const char *kActionAbort = "/abort";

QString Pin::msg_("Please enter the pin for the room, or enter /abort the cancel.");

Pin::Pin(Room *room, Client *client)
  : Protocol(), room_(room), client_(client), pass_(false) {}

Pin::~Pin() {
  qDebug() << "Pin protocol destroyed";
}

const QString *Pin::intro() const { return &msg_; }

bool Pin::execute(const QString &input, QStringList* output) {
  if (Command::ParseHead(input) == kActionAbort) {
    *output << "Room not joined";
    pass_ = true;
  } else {
    QString trial = input.trimmed();
    if (room_->pin() == trial) {
      *output << "Joining room: ";
      output->last().append(room_->name());
      emit client_->join(room_);
      pass_ = true;
    } else {
      *output << "Incorrect pin!";
      *output << msg_;
      pass_ = false;
    }
  }
  return pass_;
}

bool Pin::finished() { return pass_; }

const char *kActionRooms = "/rooms";
const char *kActionCreate = "/create";
const char *kActionQuit = "/quit";
const char *kActionJoin = "/join";

QString HallAction::actionList_("Commands you can use: /rooms, /create, /join, /quit.");

HallAction::HallAction(Hall *hall, Client *client)
  : Protocol(), hall_(hall), client_(client) {}

HallAction::~HallAction() {
  qDebug() << "HallAction protocol destroyed";
}

const QString *HallAction::intro() const { return nullptr; }

const int kMaxRoomCount = 20;

bool HallAction::execute(const QString &input, QStringList* output) {
  QString head = Command::ParseHead(input);
  if (head == kActionRooms) {
    for (auto it : hall_->opens_) {
      *output << " * ";
      output->last().append(it.first);
      if (!it.second->pin().isEmpty()) {
        output->last().append(" (** pin required)");
      }
    }
  } else if (head == kActionCreate) {
    if (Hall::opens_.size() >= kMaxRoomCount) {
      *output << QString("Maximum room count reached: %1").arg(kMaxRoomCount);
    } else {
      Create* create = new Create;
      if (!create->execute(input, output)) {
        client_->registerProtocol(create);
      } else {
        delete create;
      }
    }
  } else if (head == kActionQuit) {
    emit client_->closeConnection();
    *output << "BYE!";
  } else if (head == kActionJoin) {
    Join* join = new Join(hall_, client_);
    if (!join->execute(input, output)) {
      client_->registerProtocol(join);
    } else {
      delete join;
    }
  } else {
    *output << actionList_;
  }
  return false;
}

bool HallAction::finished() { return false; }

Join::Join(Hall *hall, Client *client)
  : Command(), hall_(hall), client_(client) {
  AddArgument("name", "the name of the room to join");
}

Join::~Join() {
  qDebug() << "Join protocol destroyed";
}

bool Join::execute(QStringList *output) {
  auto it = hall_->opens_.find(args_[0].value);
  if (it == hall_->opens_.end()) {
    *output << "Room ";
    output->last().append(args_[0].value);
    output->last().append(" doesn't exist.");
    index_ = 0;
    return false;
  } else {
    if (it->second->pin().isEmpty()) {
      *output << "Joining room: ";
      output->last().append(args_[0].value);
      emit client_->join(it->second);
    } else {
      emit client_->registerProtocol(new Pin(it->second, client_));
    }
    return true;
  }
}

Create::Create() : Command() {
  AddArgument("name", "the name of the room to create");
  AddArgument("pin", "the pin code to enter the room", true);
}

Create::~Create() {
  qDebug() << "Create protocol destroyed";
}

bool Create::execute(QStringList *output) {
  auto r = Hall::CreateRoom(args_[0].value, args_[1].value);
  if (r.second) {
    *output << "Room ";
    output->last().append(args_[0].value).append(" created.");
    return true;
  } else {
    *output << "Room ";
    output->last().append(args_[0].value);
    output->last().append(" already exists. Try another:");
    index_ = 0;
    return false;
  }
}
