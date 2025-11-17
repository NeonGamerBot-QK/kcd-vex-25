#include "main.h"
#include "globals.hpp"

void intakeForward() {
	intakeMotor.move(127);
}

void intakeBackward() {
intakeMotor.move(-127);
}

void intakeStop() {
intakeMotor.move(0);
}
