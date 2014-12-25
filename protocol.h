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

  virtual bool execute(const QString &input, QStringList *output);
  virtual bool finished();

signals:

public slots:

private:
  int priority_;
};

class Command : public Protocol {
public:
  static QString ParseHead(const QString &line);

  Command();
  ~Command() override;

  const QString *intro() const override;
  bool execute(const QString &input, QStringList *output) override;
  bool finished() override;

private:
  struct Argument {
    QString name;
    QString description;
  };

  std::vector<Argument> args_;
};

#endif // PROTOCOL_H
