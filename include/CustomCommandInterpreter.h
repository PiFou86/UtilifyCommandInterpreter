#pragma once

#include <Utilify/CommandInterpreter/BaseCommandInterpreter.h>

class CustomCommandInterpreter : public BaseCommandInterpreter {
 protected:
  virtual bool interpret(const String &command,
                         const String &parameters) override;
  virtual bool getParameter(const String &key) override;
  virtual bool setParameter(const String &key, const String &value) override;

 public:
  CustomCommandInterpreter(Stream &stream);
};