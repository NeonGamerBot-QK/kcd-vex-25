#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "main.h"

// Port Definitions
// Left side (top to bottom): 13, 14, 15
constexpr int PORT_LEFT_1 = 13;
constexpr int PORT_LEFT_2 = 14;
constexpr int PORT_LEFT_3 = 15;

// Right side (top to bottom): 4, 5, 11
constexpr int PORT_RIGHT_1 = 4;
constexpr int PORT_RIGHT_2 = 5;
constexpr int PORT_RIGHT_3 = 11;

// Intake motors (negative = reversed)
constexpr int PORT_INTAKE = 1;
constexpr int PORT_INTAKE2 = -9;
constexpr int PORT_INTAKE3 = -21;
constexpr int PORT_OUTTAKE = 11;
constexpr int PORT_OUTTAKE2 = 12;
constexpr int PORT_COLOR_OPTICAL = 19;
constexpr int PORT_STRAFE = 67; // todo: change lol
constexpr int PORT_IMU = 10;
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
extern pros::Imu imu;

// LemLib Chassis
#include "lemlib/api.hpp"
extern lemlib::Chassis chassis;

#endif
