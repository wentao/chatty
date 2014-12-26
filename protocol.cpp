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

  QString text = input.trimmed();
  if (firstExecution_) {
    firstExecution_ = false;
    // try to match all arguments from the input

    int from = 0, to = -1;
    forever {
      if (from > 0 && index_ == args_.size() - 1) {
        QString last = text.right(text.size() - from);
        if (!last.isEmpty()) {
          args_[index_++].value = last;
        }
        break;
      }

      QRegExp r(kWhitespaces);
      to = text.indexOf(r, from);
      if (to == -1 || r.matchedLength() == -1) {
        break;
      }
      if (from > 0) {
        // skip the first match (head)
        QString part = text.mid(from, to - from);
        args_[index_++].value = part;
      }
      from = to + r.matchedLength();
    }
  } else {
    args_[index_++].value = text;
  }

  bool ok = index_ == args_.size() && execute(output);
  if (!ok) {
    *output << args_[index_].description;
    output->last().prepend("Please enter ");
    output->last().append(", or type /abort to cancel the action");
  }
  return ok;
}

bool Command::finished() { return index_ == args_.size(); }
