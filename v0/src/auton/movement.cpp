#include "main.h"
#include "auton/movement.hpp"
#include "globals.hpp"

void moveForward(int speed, int duration_ms) {
	left_mg.move(speed);
	right_mg.move(speed);
	pros::delay(duration_ms);
	left_mg.move(0);
	right_mg.move(0);
}

void moveBackward(int speed, int duration_ms) {
	left_mg.move(-speed);
	right_mg.move(-speed);
	pros::delay(duration_ms);
	left_mg.move(0);
	right_mg.move(0);
}

void turnLeft(int speed, int duration_ms) {
	left_mg.move(-speed);
	right_mg.move(speed);
	pros::delay(duration_ms);
	left_mg.move(0);
	right_mg.move(0);
}

void turnRight(int speed, int duration_ms) {
	left_mg.move(speed);
	right_mg.move(-speed);
	pros::delay(duration_ms);
	left_mg.move(0);
	right_mg.move(0);
}
