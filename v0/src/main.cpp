#include "lemlib/api.hpp" // IWYU pragma: keep
#include "main.h"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// pros::lcd::initialize();
	// pros::lcd::set_text(1, "Hello PROS User!");

	// pros::lcd::register_btn1_cb(on_center_button);
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

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

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
	pros::MotorGroup left_mg({1, -2, 3});    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	pros::MotorGroup right_mg({-4, 5, -6});  // Creates a motor group with forwards port 5 and reversed ports 4 & 6
	bool telemToggle = true; // for switching tele output on controller screen
pros::Task([] { // run only in competition
		// if (field_status == "competition") {
		// 	Gif* gif = new Gif("/usd/nokotan.gif", rd_view_obj(gifview));
		// 	rd_view_focus(gifview);
		// 	console.println("Launching gif...");
		// }
	});

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control scheme
int dir = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
int turn = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
  // Gets the turn left/right from right joystick
		left_mg.move(dir - turn);                      // Sets left motor voltage
		right_mg.move(dir + turn);                     // Sets right motor voltage
			// double drivetrainTemps = ks::vector_average(leftDrive.get_temperature_all());
			double drivetrainTemps = 100;
		// double theta = fmod(chassis.getPose().theta, 360); // wrap to [0, 360) for user view
		double theta = fmod(1, 360); // wrap to [0, 360) for user view
    	if (theta < 0) {
       		theta += 360;
		}

		if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
			telemToggle = !telemToggle; // Toggle telemetry display
		}
		if(!telemToggle) {
			master.print(0, 0, "DT%.0lf|INT%.0lf|T%.0lf  ", drivetrainTemps, 0, theta);
		} else {
			// master.print(0, 0, "X:%.0lf Y:%.0lf T:%.0lf   ", chassis.getPose().x, chassis.getPose().y, theta);
			master.print(0, 0, "X:%.0lf Y:%.0lf T:%.0lf   ", 0, 1, theta);
		}
		pros::delay(20);                               // Run for 20 ms then update
	}
}