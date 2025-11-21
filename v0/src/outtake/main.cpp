#include "main.h"
#include "globals.hpp"

// Pneumatics (Port A)
#define OUTTAKE_PISTON_PORT 'A'
pros::ADIDigitalOut outtakePiston(OUTTAKE_PISTON_PORT);
bool outtakePistonState = false;

void toggleOuttakePiston() {
    outtakePistonState = !outtakePistonState;
    outtakePiston.set_value(outtakePistonState);
}

void setOuttakePiston(bool state) {
    outtakePistonState = state;
    outtakePiston.set_value(state);
}

// Motor (Port 8)
void outtakeForward() {
    outtakeMotor.move(127);
}

void outtakeBackward() {
    outtakeMotor.move(-127);
}

void outtakeStop() {
    outtakeMotor.move(0);
}
