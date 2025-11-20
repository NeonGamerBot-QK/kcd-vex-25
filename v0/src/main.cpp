#include "lemlib/api.hpp" // IWYU pragma: keep
#include "main.h"

#include "screen/init.hpp"
#include "auton/movement.hpp"
#include "intake/pneumatics.hpp"
#include "globals.hpp"

void startOdometryTask();

const char* get_auton_name(int auton) {
	switch(auton) {
		case 0: return "Red Left";
		case 1: return "Red Right";
		case 2: return "Blue Left";
		case 3: return "Blue Right";
		case 4: return "Skills";
		case 5: return "None";
		default: return "Unknown";
	}
}

// PID Logging Task
void pid_log_task(void* param) {
	printf("Time,X,Y,Theta,Auton\n"); // CSV Header
	while (true) {
		lemlib::Pose p = chassis.getPose();
		int auton = get_selected_auton();
		const char* name = get_auton_name(auton);
		
		if (pros::competition::is_connected()) {
			printf("%d,%.2f,%.2f,%.2f,COMP\n", pros::millis(), p.x, p.y, p.theta);
		} else {
			printf("%d,%.2f,%.2f,%.2f,%s\n", pros::millis(), p.x, p.y, p.theta, name);
		}
		pros::delay(100);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	screen_init();
	chassis.calibrate(); // Calibrate chassis sensors
	
	// Start logging task
	pros::Task logger(pid_log_task, nullptr, "PID Logger");
	
	// startOdometryTask(); // Removed in favor of LemLib's internal odometry
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
			moveBackward(100, 100);
			// intake balls in here
			// TODO: remove
			pros::delay(1000);
			moveForward(100, 400);
			// implement functionaliy for outake here (ts will send balls into the bridge)
			// wait till outtake is done
			// todo: remove this
			pros::delay(5000);
			moveBackward(100, 100);
			turnRight(80, 400);
			moveForward(100, 400);
			// dispense balls here again..
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
			moveForward(100, 200);
			// sleep 2
			pros::delay(1000);
			turnLeft(80, 500);
			// moveForward(100, 1000);
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

		// Motor connection check
		static pros::Motor l1(PORT_LEFT_1);
		static pros::Motor l2(PORT_LEFT_2);
		static pros::Motor l3(PORT_LEFT_3);
		static pros::Motor r1(PORT_RIGHT_1);
		static pros::Motor r2(PORT_RIGHT_2);
		static pros::Motor r3(PORT_RIGHT_3);
		
		bool motor_err = false;
		
		if (!l1.is_installed()) { 
			printf("L1 (%d) DC!\n", PORT_LEFT_1); 
			master.print(0, 0, "Err: L1 (%d)", PORT_LEFT_1); 
			master.rumble("."); 
			motor_err = true;
		} else if (!l2.is_installed()) { 
			printf("L2 (%d) DC!\n", PORT_LEFT_2); 
			master.print(0, 0, "Err: L2 (%d)", PORT_LEFT_2); 
			master.rumble("."); 
			motor_err = true;
		} else if (!l3.is_installed()) { 
			printf("L3 (%d) DC!\n", PORT_LEFT_3); 
			master.print(0, 0, "Err: L3 (%d)", PORT_LEFT_3); 
			master.rumble("."); 
			motor_err = true;
		} else if (!r1.is_installed()) { 
			printf("R1 (%d) DC!\n", PORT_RIGHT_1); 
			master.print(0, 0, "Err: R1 (%d)", PORT_RIGHT_1); 
			master.rumble("."); 
			motor_err = true;
		} else if (!r2.is_installed()) { 
			printf("R2 (%d) DC!\n", PORT_RIGHT_2); 
			master.print(0, 0, "Err: R2 (%d)", PORT_RIGHT_2); 
			master.rumble("."); 
			motor_err = true;
		} else if (!r3.is_installed()) { 
			printf("R3 (%d) DC!\n", PORT_RIGHT_3); 
			master.print(0, 0, "Err: R3 (%d)", PORT_RIGHT_3); 
			motor_err = true;
		}
		if(motor_err) {
			master.rumble("."); 
		}
		 // ignore intake
		//  else if (!intakeMotor.is_installed()) { 
		// 	printf("Intake (%d) DC!\n", PORT_INTAKE); 
		// 	master.print(0, 0, "Err: Intake"); 
		// 	master.rumble("."); 
		// 	motor_err = true;
		// }
		
		if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
			telemToggle = !telemToggle; // Toggle telemetry display
			toggleIntakeLift();
		}
		if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN) && !pros::competition::is_connected()) {
			master.rumble("- -");
			autonomous();
		}
		
		if (!motor_err) {
			if(!telemToggle) {
				master.print(0, 0, "DT%.0lf|INT%.0lf|T%.0lf  ", drivetrainTemps, 0, theta);
			} else {
				lemlib::Pose p = chassis.getPose();
				const char* name = get_auton_name(get_selected_auton());
				char short_name[10];
				strncpy(short_name, name, 4); // First 4 chars
				short_name[4] = '\0';
				
				master.print(0, 0, "X:%.0f Y:%.0f %s   ", p.x, p.y, name);
			}
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