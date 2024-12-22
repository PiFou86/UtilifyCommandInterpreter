#include <Arduino.h>

#include "CustomCommandInterpreter.h"

CustomCommandInterpreter commandInterpreter(Serial);
void setup() { ; }

void loop() { commandInterpreter.tick(); }