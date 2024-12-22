#pragma once

#include <Arduino.h>
#include <Stream.h>

#include <Utilify/CommandInterpreter/HasOptionalLibs.h>

class BaseCommandInterpreter {
 private:
  Stream &m_stream;
  String m_lastSerialInput;

 protected:
  virtual bool interpret(const String &command, const String &parameters);
  virtual bool getParameter(const String &key);
  virtual bool setParameter(const String &key, const String &value);

 public:
  BaseCommandInterpreter(Stream &stream);
  void tick();
  bool executeCommand(const String &command);
};