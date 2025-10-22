#include "main.h"

pros::Motor intakeMotor(7);

void intakeForward() {
intakeMotor.move(127);
}

void intakeBackward() {
intakeMotor.move(-127);
}

void intakeStop() {
intakeMotor.move(0);
}

