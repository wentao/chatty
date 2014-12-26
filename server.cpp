#include "server.h"

Server::Server(QObject *parent) : QObject(parent), looper_(parent) {}

Server::~Server() {}

void Server::startLooper() {
  looper_.start();
  moveToThread(&looper_);
}
