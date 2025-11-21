#include "main.h"
#include "globals.hpp"

pros::MotorGroup left_mg({PORT_LEFT_1, PORT_LEFT_2, PORT_LEFT_3});
pros::MotorGroup right_mg({-PORT_RIGHT_1, -PORT_RIGHT_2, -PORT_RIGHT_3});
pros::Motor intakeMotor(PORT_INTAKE);
pros::Motor intakeMotor2(PORT_INTAKE2);
pros::Motor outtakeMotor(PORT_OUTTAKE);
pros::Optical colorSensor(PORT_COLOR_OPTICAL);

// Drivetrain Settings
lemlib::Drivetrain drivetrain(&left_mg, &right_mg, 10.0, lemlib::Omniwheel::NEW_275, 450, 2);

// PID Settings
lemlib::ControllerSettings lateral_controller(10, 0, 3, 3, 1, 100, 3, 500, 20);
lemlib::ControllerSettings angular_controller(2, 0, 10, 3, 1, 100, 3, 500, 20);

// Sensors (Odom)
lemlib::OdomSensors sensors(nullptr, nullptr, nullptr, nullptr, nullptr);

// Chassis
lemlib::Chassis chassis(drivetrain, lateral_controller, angular_controller, sensors);
