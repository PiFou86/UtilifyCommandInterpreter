#include <Arduino.h>

#include "CustomCommandInterpreter.h"

CustomCommandInterpreter::CustomCommandInterpreter(Stream &stream)
    : BaseCommandInterpreter(stream) {
  addCommand("test", "[parameters]");
}

bool CustomCommandInterpreter::interpret(const String &command,
                                           const String &parameters) {
  bool error = false;

  if (command == "test") {
    Serial.println(String(F("test command executed with parameters: ")) + parameters);
  } else {
    error = !BaseCommandInterpreter::interpret(command, parameters);
  }

  return !error;
}

bool CustomCommandInterpreter::getParameter(const String &key) {
  // Custom code here to handle custom keys and values
  return BaseCommandInterpreter::getParameter(key);
}

bool CustomCommandInterpreter::setParameter(const String &key,
                                              const String &value) {
  // Custom code here to handle custom keys and values
  return BaseCommandInterpreter::setParameter(key, value);
}
