#include "protocol.h"

Protocol::Protocol() {}

Protocol::~Protocol() {}

const QString* Protocol::intro() const { return nullptr; }

bool Protocol::execute(const QString &input, QString *output) { return true; }
bool Protocol::finished() { return true; }
