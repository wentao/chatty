#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <functional>

#include <QObject>

class Protocol {
public:
  Protocol();
  virtual ~Protocol();

  virtual const QString *intro() const;

  virtual bool execute(const QString &input, QString *output);
  virtual bool finished();

signals:

public slots:

private:
  int priority_;
};

class Command : public Protocol {
public:
  Command();
  ~Command() override;

  bool execute(const QString &input, QString *output) override;
  bool finished() override;

private:
  struct Argument {
    QString name;
    QString description;
    std::function<bool(const QString &)> validator;
  };

  std::vector<Argument> args_;
};

#endif // PROTOCOL_H
