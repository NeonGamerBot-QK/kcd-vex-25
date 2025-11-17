#include "lemlib/api.hpp" // IWYU pragma: keep
#include "main.h"

#include "screen/init.hpp"
#include "auton/movement.hpp"
#include "intake/pneumatics.hpp"
#include "globals.hpp"

void startOdometryTask();

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	screen_init();
	startOdometryTask();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

void autonomous() {
	int auton = get_selected_auton();
	
	switch(auton) {
		case 0: // Red Left - AWP
			moveForward(100, 1000);
			turnRight(80, 500);
			moveForward(100, 1000);
			break;
		
		case 1: // Red Right - Score
			moveForward(100, 1200);
			turnLeft(80, 500);
			moveBackward(100, 800);
			break;
		
		case 2: // Blue Left - Score
			moveForward(100, 1200);
			turnRight(80, 500);
			moveBackward(100, 800);
			break;
		
		case 3: // Blue Right - AWP
			moveForward(100, 1000);
			turnLeft(80, 500);
			moveForward(100, 1000);
			break;
		
		case 4: // Skills Run
			moveForward(127, 2000);
			turnRight(100, 1000);
			moveForward(127, 2000);
			break;
		
		case 5: // Do Nothing
		default:
			break;
	}
}


/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	bool telemToggle = true;

	while (true) {
		int dir = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		int turn = -master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
		
		left_mg.move(dir + turn);
		right_mg.move(dir - turn);
		
		double drivetrainTemps = left_mg.get_temperature();
		double theta = fmod(1, 360);
		if (theta < 0) {
			theta += 360;
		}

		if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
			telemToggle = !telemToggle; // Toggle telemetry display
			toggleIntakeLift();
		}
		if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN) && !pros::competition::is_connected()) {
			master.rumble("- -");
			autonomous();
		}
		
		if(!telemToggle) {
			master.print(0, 0, "DT%.0lf|INT%.0lf|T%.0lf  ", drivetrainTemps, 0, theta);
		} else {
			// master.print(0, 0, "X:%.0lf Y:%.0lf T:%.0lf   ", chassis.getPose().x, chassis.getPose().y, theta);
			master.print(0, 0, "X:%.0lf Y:%.0lf T:%.0lf   ", 0, 1, theta);
		}
		if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
			intakeMotor.move(127); // Intake forward
		} else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
			intakeMotor.move(-127); // Intake backward
		} else {
			intakeMotor.move(0); // Intake stop
		}

		pros::delay(20);               // Run for 20 ms then update
	}
}