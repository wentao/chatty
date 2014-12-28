#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include <functional>

#include <QObject>

// The abstract class of how client would communicate with the server.
class Protocol {
public:
  Protocol();
  virtual ~Protocol();

  // The introduction message to be sent to the client when this protocol
  // is used for the first time.
  virtual const QString *intro() const;

  // Processes the input message received from the client and generate the output
  // message that will be sent back to the client later.
  //
  // This method will return true if the execution is finished, which means
  // the client will remove this protocol from its protocols stack. Otherwise,
  // this protocol will continue to be executed after a new message is received
  // from the client.
  virtual bool execute(const QString &input, QStringList *output);

  // Checks if the protocol finishes the execution.
  virtual bool finished();
};

// A command-like protocol allows the user to enter arguments either in
// a single message or multiple-step messages. For example, for command
//
//   /whisper <to> <msg>
//
// The client can enter arguments in one of the following ways:
//
//   1-step: /whisper <to> <msg>
//   2-step: /whisper <to> + <msg>
//   3-step: /whisper + <to> + <msg>
//
// Subclasses must use AddArgument() to add arguments to be used for the
// command and override the execute(QStringList) method to generate the
// output string.
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

    // Wheather the argument can be skipped (with empty string value).
    bool optional;

    // Wheather the argument should be a token (no whitespaces allowed
    // in the middle fo the string value).
    bool token;

    Argument(const QString &name,
             const QString &description,
             bool optional = false);
  };

  Argument *AddArgument(const QString &n, const QString &desc, bool optional = false);

  // This method will be called when all arguments values are fulfilled.
  virtual bool execute(QStringList *output) = 0;

  std::vector<Argument> args_;
  size_t index_;
  bool firstExecution_;
};

#endif // PROTOCOL_H
