#include "main.h"
#include "auton/movement.hpp"

void moveForward(int speed, int duration_ms) {
	pros::MotorGroup left_mg({-1, -2, -3});
	pros::MotorGroup right_mg({4, 5, 6});
	
	left_mg.move(speed);
	right_mg.move(speed);
	pros::delay(duration_ms);
	
	left_mg.move(0);
	right_mg.move(0);
}

void moveBackward(int speed, int duration_ms) {
	pros::MotorGroup left_mg({-1, -2, -3});
	pros::MotorGroup right_mg({4, 5, 6});
	
	left_mg.move(-speed);
	right_mg.move(-speed);
	pros::delay(duration_ms);
	
	left_mg.move(0);
	right_mg.move(0);
}

void turnLeft(int speed, int duration_ms) {
	pros::MotorGroup left_mg({-1, -2, -3});
	pros::MotorGroup right_mg({4, 5, 6});
	
	left_mg.move(-speed);
	right_mg.move(speed);
	pros::delay(duration_ms);
	
	left_mg.move(0);
	right_mg.move(0);
}

void turnRight(int speed, int duration_ms) {
	pros::MotorGroup left_mg({-1, -2, -3});
	pros::MotorGroup right_mg({4, 5, 6});
	
	left_mg.move(speed);
	right_mg.move(-speed);
	pros::delay(duration_ms);
	
	left_mg.move(0);
	right_mg.move(0);
}
