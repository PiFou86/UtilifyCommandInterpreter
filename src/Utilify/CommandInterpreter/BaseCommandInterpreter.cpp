#include <Arduino.h>
#include <Stream.h>
#include <Utilify/CommandInterpreter/BaseCommandInterpreter.h>
#include <Utilify/CommandInterpreter/Device.h>
#include <Utilify/StringUtils.h>

#ifdef ESP32
#include <Utilify/CommandInterpreter/ESP32Utils.h>

#include <vector>
#endif

#ifdef UTILIFY_LOGGER
#include <Utilify/Logger/Logger.h>
#endif

BaseCommandInterpreter::BaseCommandInterpreter(Stream &stream)
    : m_stream(stream), m_lastSerialInput("") {
  addCommand("echo", "<message>");
  addCommand("set", "<key> <value>");
  addCommand("get", "<key>");
  addCommand("id", "");
#ifdef ESP32
  addCommand("reboot", "");
  addCommand("scan", "i2c");
  addCommand("scan", "wifi");
  addCommand("flash", "");
  addCommand("network", "");
#elif ARDUINO_AVR_UNO
  addCommand("scan", "i2c");
#endif
  addCommand("help", "");
}

void BaseCommandInterpreter::begin() {
  m_stream.println(F("Welcome to Utilify Command Interpreter"));
  m_stream.println(F("Type 'help' to get a list of available commands"));
  m_stream.println();
  m_stream.print(F("# "));
}

void BaseCommandInterpreter::printHelp() {
  m_stream.println(F("Available commands:"));
  for (size_t i = 0; i < this->m_commands.size(); i++) {
    if (this->m_commands[i].parameters.size() == 0) {
      m_stream.print(F("  "));
      m_stream.println(this->m_commands[i].command);
    } else {
      for (size_t j = 0; j < this->m_commands[i].parameters.size(); j++) {
        m_stream.print(F("  "));
        m_stream.print(this->m_commands[i].command);
        m_stream.print(F(" "));
        m_stream.println(this->m_commands[i].parameters[j]);
      }
    }
  }
}

void BaseCommandInterpreter::tick() {
  while (this->m_stream.available()) {
    char c = this->m_stream.read();
    this->m_stream.print(c);
    if (c == '\n') {
      m_lastSerialInput.trim();
      this->executeCommand(m_lastSerialInput);
      this->m_lastSerialInput = "";
      m_stream.print(F("# "));
    } else if (c == '\b') {
      if (this->m_lastSerialInput.length() > 0) {
        this->m_lastSerialInput = this->m_lastSerialInput.substring(
            0, this->m_lastSerialInput.length() - 1);
      }
    } else if (c == '\t') {  // Détection de la touche Tab
      handleAutocomplete();
    } else {
      this->m_lastSerialInput += c;
    }
  }
}

bool BaseCommandInterpreter::executeCommand(const String &command) {
  String cmd = command;

  String parameters = "";
  int index = cmd.indexOf(' ');
  if (index != -1) {
    parameters = cmd.substring(index + 1);
    cmd = cmd.substring(0, index);
  }
  cmd.trim();
  parameters.trim();

  return this->interpret(cmd, parameters);
}

bool BaseCommandInterpreter::interpret(const String &command,
                                       const String &parameters) {
  bool error = false;

  if (command == "echo") {
    m_stream.print(F("echo : "));
    m_stream.println(parameters);
  } else if (command == "set") {
    String key = parameters.substring(0, parameters.indexOf(' '));
    String value = parameters.substring(parameters.indexOf(' ') + 1);
    error = !this->setParameter(key, value);
  } else if (command == "get") {
    String key = parameters;
    error = !this->getParameter(key);
  } else if (command == "id") {
    m_stream.println(String(F("ID: ")) + Device::getId());
  }
#ifdef ESP32
  else if (command == "reboot") {
    m_stream.println(F("Rebooting..."));
    ESP.restart();
  }
#endif
  else if (command == "42") {
    m_stream.println(F("Beaucoup se sont perdus dans mes ailes; tous n'ont pas vu les mêmes choses, mais la folie était l'une d'entre elles !"));
    m_stream.println(F("Many have lost themselves in my wings; not all saw the same things, but madness was one of them !"));
    m_stream.println(F("Muchos se han perdido en mis alas; no todos vieron las mismas cosas, pero la locura fue una de ellas !"));
    m_stream.println(F("Muitos se perderam em minhas asas; nem todos viram as mesmas coisas, mas a loucura foi uma delas !"));
    m_stream.println(F("Molti si sono persi nelle mie ali; non tutti hanno visto le stesse cose, ma la follia era una di esse !"));
    m_stream.println(F("Viele haben sich in meinen Flügeln verloren; nicht alle sahen dasselbe, aber der Wahnsinn war einer davon !"));
  } else if (command == "scan") {
    String deviceType = parameters;

    if (deviceType == "i2c") {
#ifdef ARDUINO_AVR_UNO
      SimpleCollection<uint16_t> i2cAddresses = Device::getI2CAddresses();
#elif ESP32
      std::vector<uint16_t> i2cAddresses = Device::getI2CAddresses();
#endif
      m_stream.println(F("I2C addresses:"));
      for (size_t i = 0; i < i2cAddresses.size(); i++) {
        m_stream.println(
            String(F("  - 0x")) +
            StringUtils::padLeft(String(i2cAddresses[i], HEX), 2, '0'));
      }
      m_stream.println(String(F("")));
    }
#ifdef ESP32
    else if (deviceType == "wifi") {
      printWiFiScan(m_stream);
    }
#endif
    else {
      m_stream.println(String(F("Unknown device type ")) + deviceType);
    }
  }
#ifdef ESP32
  else if (command == "flash") {
    m_stream.println(String(F("Flash size: ")) + ESP.getFlashChipSize() +
                     String(F(" bytes")));
    m_stream.println(String(F("Flash speed: ")) + ESP.getFlashChipSpeed() +
                     String(F(" Hz")));
    m_stream.println(String(F("Flash mode: ")) + ESP.getFlashChipMode());

    printPartitionInfo(m_stream);
  } else if (command == "network") {
    printWiFiInfo(m_stream);
  }
#endif
  else if (command == "help") {
    printHelp();
  } else if (command != "") {
    m_stream.println(String(F("Unknown command: ")) + command);
    error = true;
  }

  return !error;
}

bool BaseCommandInterpreter::getParameter(const String &key) {
  bool error = false;
  bool keyFound = false;

#ifdef UTILIFY_LOGGER
  if (key == "debug") {
    keyFound = true;
    m_stream.println(String(F("debug: ")) + Logger.getLoggerLevel());
  }
#endif

  if (!keyFound) {
    m_stream.println(F("get : Invalid key"));
    error = true;
  }

  return !error;
}

bool BaseCommandInterpreter::setParameter(const String &key,
                                          const String &value) {
  bool error = false;
  bool keyFound = false;

#ifdef UTILIFY_LOGGER
  if (key == "debug") {
    keyFound = true;
    int debugLevel = value.toInt();
    if (debugLevel > 0 && debugLevel <= 4) {
      Logger.setLoggerLevel((LoggerLevel)debugLevel);
    } else {
      m_stream.println(F("Invalid debug level"));
    }
  }
#endif

  if (!keyFound) {
    m_stream.println(F("set : Invalid key"));
    error = true;
  }

  return !error;
}

void BaseCommandInterpreter::addCommand(const String &command,
                                        const String &parameters) {
  int commandIndex = -1;
  for (size_t i = 0; i < this->m_commands.size() && commandIndex == -1; i++) {
    if (this->m_commands[i].command == command) {
      commandIndex = i;
    }
  }

  if (commandIndex == -1) {
    Command cmd;
    cmd.command = command;
    this->m_commands.push_back(cmd);
    commandIndex = this->m_commands.size() - 1;
  }

  if (parameters != "") {
    int parameterIndex = -1;
    for (size_t i = 0; i < this->m_commands[commandIndex].parameters.size() &&
                       parameterIndex == -1;
         i++) {
      if (this->m_commands[commandIndex].parameters[i] == parameters) {
        parameterIndex = i;
      }
    }
    if (parameterIndex == -1) {
      this->m_commands[commandIndex].parameters.push_back(parameters);
    }
  }
}

void BaseCommandInterpreter::handleAutocomplete() {
  String input = m_lastSerialInput;
  input.trim();

  int spaceIndex = input.indexOf(' ');
  String mainCommand = input;
  String subCommand = "";
  if (spaceIndex != -1) {
    mainCommand = input.substring(0, spaceIndex);
    subCommand = input.substring(spaceIndex + 1);
    subCommand.trim();
  }

#ifdef ESP32
  std::vector<String> matches;
#else
  SimpleCollection<String> matches;
#endif

  if (subCommand == "") {
    for (size_t i = 0; i < m_commands.size(); i++) {
      if (m_commands[i].command.startsWith(mainCommand)) {
        matches.push_back(m_commands[i].command);
      }
    }
  } else {
    for (size_t i = 0; i < m_commands.size(); i++) {
      if (m_commands[i].command == mainCommand) {
        for (size_t j = 0; j < m_commands[i].parameters.size(); j++) {
          if (m_commands[i].parameters[j].startsWith(subCommand)) {
            matches.push_back(m_commands[i].parameters[j]);
          }
        }
        break;
      }
    }
  }

  // if (matches.empty()) {
  //   m_stream.println(F("\nNo matches found"));
  // } else
  if (matches.size() == 1) {
    if (subCommand == "") {
      m_lastSerialInput = matches[0] + " ";
    } else {
      m_lastSerialInput = mainCommand + " " + matches[0] + " ";
    }
    m_stream.print("\r# ");
    m_stream.print(m_lastSerialInput);
  } else {
    m_stream.println();
    for (size_t i = 0; i < matches.size(); i++) {
      m_stream.println("  " + matches[i]);
    }
    m_stream.print("# ");
    m_stream.print(m_lastSerialInput);
  }
}