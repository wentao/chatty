#include "protocol.h"

#include <QDebug>
#include <QRegExp>

Protocol::Protocol() {}

Protocol::~Protocol() {}

const QString* Protocol::intro() const { return nullptr; }

bool Protocol::execute(const QString &input, QStringList *output) { return true; }
bool Protocol::finished() { return true; }

QString Command::ParseHead(const QString &line) {
  QString head = line.section(QRegExp("\\s+"), 0, 0, QString::SectionSkipEmpty);
  qDebug() << "head" << head;
  return head;
}
