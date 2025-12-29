#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "main.h"

// Port Definitions
constexpr int PORT_LEFT_1 = 13;
constexpr int PORT_LEFT_2 = 4;
constexpr int PORT_LEFT_3 = 20;

constexpr int PORT_RIGHT_1 = 1;
constexpr int PORT_RIGHT_2 = 15;
constexpr int PORT_RIGHT_3 = 3;

constexpr int PORT_INTAKE = 6; // bottom
constexpr int PORT_INTAKE2 = 7;
constexpr int PORT_INTAKE3 = 2;
constexpr int PORT_OUTTAKE = 11;
constexpr int PORT_OUTTAKE2 = 12;
constexpr int PORT_COLOR_OPTICAL = 19;
constexpr int PORT_STRAFE = 67; // todo: change lol
constexpr int PORT_IMU = 19;
// Config
#define SHOW_COLOR_ON_SCREEN 1

extern pros::MotorGroup left_mg;
extern pros::MotorGroup right_mg;
extern pros::Motor intakeMotor;
extern pros::Motor intakeMotor2;
extern pros::Motor intakeMotor3;
extern pros::Motor outtakeMotor;
extern pros::Motor outtakeMotor2;
extern pros::Optical colorSensor;
extern pros::Motor strafeMotor;


// LemLib Chassis
#include "lemlib/api.hpp"
extern lemlib::Chassis chassis;

#endif
