#include "main.h"
#define DIGITAL_SENSOR_PORT 'A'
bool lastPistonValue = false;
pros::ADIDigitalOut piston (DIGITAL_SENSOR_PORT);
void toggleIntakeLift() {
lastPistonValue = !lastPistonValue;
  piston.set_value(lastPistonValue);
}
void stopPiston() {
    lastPistonValue = false;
    piston.set_value(false);
}
void startPiston() {
    lastPistonValue = true;
    piston.set_value(true);
}