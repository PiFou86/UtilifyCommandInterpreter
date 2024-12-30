#pragma once

#include <Arduino.h>
#include <Stream.h>

#ifdef ESP32
#include <vector>
#endif

#include <Utilify/CommandInterpreter/HasOptionalLibs.h>

#include <Utilify/BasicVector.h>

struct Command {
 public:
  String command;
  vector<String> parameters;
};

class BaseCommandInterpreter {
 private:
  Stream &m_stream;
  String m_lastSerialInput;
  vector<Command> m_commands;

  void handleAutocomplete();

 protected:
  virtual bool interpret(const String &command, const String &parameters);
  virtual bool getParameter(const String &key);
  virtual bool setParameter(const String &key, const String &value);
  void addCommand(const String &command, const String &parameters);
  virtual void printHelp();

 public:
  BaseCommandInterpreter(Stream &stream);
  virtual void begin();
  void tick();
  bool executeCommand(const String &command);
};
