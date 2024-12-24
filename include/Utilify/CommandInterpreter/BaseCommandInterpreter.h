#pragma once

#include <Arduino.h>
#include <Stream.h>

#ifdef ESP32
#include <vector>
#endif

#include <Utilify/CommandInterpreter/HasOptionalLibs.h>

#include <Utilify/CommandInterpreter/SimpleCollection.h>

struct Command {
 public:
  String command;
#ifdef ESP32
  std::vector<String> parameters;
#else
  SimpleCollection<String> parameters;
#endif
};

class BaseCommandInterpreter {
 private:
  Stream &m_stream;
  String m_lastSerialInput;
  #ifdef ESP32
  std::vector<Command> m_commands;
  #else
  SimpleCollection<Command> m_commands;
  #endif

  void handleAutocomplete();

 protected:
  virtual bool interpret(const String &command, const String &parameters);
  virtual bool getParameter(const String &key);
  virtual bool setParameter(const String &key, const String &value);
  void addCommand(const String &command, const String &parameters);
  virtual void printHelp();

 public:
  BaseCommandInterpreter(Stream &stream);
  void tick();
  bool executeCommand(const String &command);
};
