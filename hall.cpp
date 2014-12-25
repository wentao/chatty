#include "hall.h"

#include "client.h"

#include <QStringList>

Hall Hall::hall_;

std::map<QString, Room *> Hall::opens_;

Hall::Hall(QObject *parent) : Room("", parent) {
  connect(this, &Hall::newConnection, &Hall::newClient);
}

Hall::~Hall() {}

std::pair<Room *, bool> Hall::CreateRoom(const QString &name) {
  auto it = opens_.find(name);
  if (it == opens_.end()) {
    Room *room = new Room(name);
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

QString Login::welcome_("Welcome to chatty, please tell us your name:");

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
    msg.append("Sorry, ");
    msg.append(name);
    msg.append(" already taken, try another?");
    *output << msg;
    return false;
  } else if (name.length() == 0) {
    msg.append("Name can't be empty!");
    *output << msg;
    return false;
  } else {
    msg.append("Welcome ");
    msg.append(name);
    msg.append("!");
    *output << msg;
    name_ = name;
    hall_->users_[name] = client_;
    client_->setName(name);
    return true;
  }
}

bool Login::finished() { return name_.length() > 0; }

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

bool HallAction::execute(const QString &input, QStringList* output) {
  QString head = Command::ParseHead(input);
  if (head == kActionRooms) {
    for (auto it : hall_->opens_) {
      *output << it.first;
    }
  } else if (head == kActionCreate) {
    *output << "create";
  } else if (head == kActionQuit) {
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
    *output << "Joining room: ";
    output->last().append(args_[0].value);
    client_->join(it->second);
    return true;
  }
}

