# UtilifyCommandInterpreter

UtilifyCommandInterpreter is a lightweight and flexible library designed for ESP32 and Arduino Uno. It provides an intuitive command-line interface for embedded systems, enabling users to interact with devices via serial input. Define, parse, and execute custom commands efficiently with this library.

## Features

- Lightweight and easy to integrate.
- Supports ESP32 and Arduino Uno platforms.
- Provides built-in commands such as echo, set, get, help, and more.
- Extensible: Define custom commands and handle parameters.
- Autocompletion support with the Tab key.
- Includes utility commands like scan, network, flash, and reboot (for ESP32).

## Getting Started

### Installation

Add the library to your PlatformIO project by including the following in your `platformio.ini` file:

```ini
[env:esp32dev]
platform = espressif32
framework = arduino
lib_deps =
    https://github.com/PiFou86/Utilify.git
    https://github.com/PiFou86/UtilifyCommandInterpreter.git
```

### Basic Usage

Create a new sketch and include the library:

```cpp
#include <Utilify/CommandInterpreter/BaseCommandInterpreter.h>

// Initialize the command interpreter
BaseCommandInterpreter commandInterpreter(Serial);

void setup() {
    Serial.begin(115200);
    commandInterpreter.begin();
}

void loop() {
    // Process incoming serial commands
    commandInterpreter.tick();
}
```

### Built-in Commands

The library comes with several built-in commands that you can use out of the box. Here are some examples:

| Command | Parameters | Description |
|---------|------------|-------------|
| help    | None       | Displays a list of available commands. |
| echo    | `<message>`| Prints back the provided message. |
| set     | `<key> <value>` | Sets a custom parameter. |
| get     | `<key>`    | Retrieves the value of a custom parameter. |
| id      | None       | Displays the device ID. |
| scan    | `i2c` or `wifi` | Scans for I2C devices or Wi-Fi networks (ESP32 only). |
| reboot  | None       | Reboots the device (ESP32 only). |
| flash   | None       | Displays information about the flash memory (ESP32 only). |
| network | None       | Displays network configuration (ESP32 only). |

### Autocompletion

The library supports autocompletion using the Tab key. If multiple matches are found, it lists all possible options.

Example Input:

```
# sc<Tab>
```

Output:
```
# scan
```

### Example Usage

```
Welcome to Utilify Command Interpreter
Type 'help' to get a list of available commands

# echo "Vive les crêpes !"
echo : "Vive les crêpes !"
# id 
ID: 5eb7dbee
# network
WiFi STA:
  - SSID: Erable
  - Password: TastyMapleSyrup_ChocolatineIsTheBest
  - IP: 192.168.3.214
  - Netmask: 255.255.0.0
  - Gateway: 192.168.3.1
  - MAC: BC:DC:A7:3F:D4:D4
  - BSSID: C4:EB:F4:F7:8E:A1
  - DNS 0: 192.168.3.1
  - DNS 1: 0.0.0.0

WiFi AP:
  - SSID: Erable
  - Password: TastyMapleSyrup_ChocolatineIsTheBest
  - IP: 192.168.23.1
  - Netmask: 255.255.255.0
  - Gateway: 192.168.23.1
  - MAC: BC:DC:A7:3F:D4:D6
  - BSSID: BC:DC:A7:3F:D4:D6

# s
  set
  scan
```

### Advanced Usage - Adding Custom Commands

You can extend the library by adding custom commands and their behavior. 

For example:

```cpp
#include <Utilify/CommandInterpreter/BaseCommandInterpreter.h>

class CustomCommandInterpreter : public BaseCommandInterpreter {
public:
    CustomCommandInterpreter(Stream &stream) : BaseCommandInterpreter(stream) {
        addCommand("test", "[parameters]");
    }
    bool interpret(const String &command, const String &parameters) {
        bool error = false;

        if (command == "test") {
            Serial.println(String(F("test command executed with parameters: ")) + parameters);
        } else {
            error = !BaseCommandInterpreter::interpret(command, parameters);
        }

        return !error;
    }
};

CustomCommandInterpreter commandInterpreter(Serial);
void setup() {
    Serial.begin(115200);
    commandInterpreter.begin();

    // Add a custom command
    commandInterpreter.addCommand("custom", "<param>");
}
```

### Dependencies

The library depends on the following:

- Utilify: Core utilities for string manipulation and device-specific operations.
- WiFi: Required for network-related commands on ESP32.
- Wire: Required for I2C-related commands.

### Author

**Pierre-François Léon**  
🔗 [Linkedin](https://www.linkedin.com/in/pierrefrancoisleon/)  
🔗 [GitHub Profile](https://github.com/PiFou86)
