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

protected:
  struct Argument {
    QString name;
    QString description;
    QString value;

    Argument(const QString &n, const QString &desc);
  };

  void AddArgument(const QString &n, const QString &desc);

  // This method will be called when all arguments values are fulfilled.
  virtual bool execute(QStringList *output) = 0;

  std::vector<Argument> args_;
  size_t index_;
  bool firstExecution_;
};

#endif // PROTOCOL_H
