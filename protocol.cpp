#include "protocol.h"

#include <QDebug>
#include <QRegExp>

Protocol::Protocol() {}
Protocol::~Protocol() {}

const QString* Protocol::intro() const { return nullptr; }
bool Protocol::execute(const QString &input, QStringList *output) { return true; }
bool Protocol::finished() { return true; }

const QRegExp kWhitespaces("\\s+");
const char *kAbort = "/abort";

QString Command::ParseHead(const QString &line) {
  QString head = line.section(kWhitespaces, 0, 0, QString::SectionSkipEmpty);
  qDebug() << "head" << head;
  return head;
}

Command::Command() : Protocol(), index_(0), firstExecution_(true) {}
Command::~Command() {}

Command::Argument::Argument(const QString &n, const QString &desc)
  : name(n), description(desc) {}

void Command::AddArgument(const QString &n, const QString &desc) {
  args_.emplace_back(n, desc);
}

const QString* Command::intro() const { return nullptr; }

bool Command::execute(const QString &input, QStringList *output) {
  if (Command::ParseHead(input) == kAbort) {
    *output << "Action cancelled.";
    return true;
  }

  if (firstExecution_) {
    firstExecution_ = false;
    QStringList args = input.split(kWhitespaces, QString::SkipEmptyParts);
    args.removeFirst(); // remove head
    while (index_ < args_.size() && !args.isEmpty()) {
      args_[index_].value = args.first();
      args.removeFirst();
      ++index_;
    }
  } else {
    args_[index_].value = input.trimmed();
    ++index_;
  }

  qDebug() << "index" << index_;
  bool ok = index_ == args_.size() && execute(output);
  if (!ok) {
    *output << args_[index_].description;
    output->last().prepend("Please enter ");
    output->last().append(", or type /abort to cancel the action");
  }
  return ok;
}

bool Command::finished() { return index_ == args_.size(); }
