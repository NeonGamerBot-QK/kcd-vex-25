#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "main.h"

// Port Definitions
constexpr int PORT_LEFT_1 = 13;
constexpr int PORT_LEFT_2 = 4;
constexpr int PORT_LEFT_3 = 6;

constexpr int PORT_RIGHT_1 = 1;
constexpr int PORT_RIGHT_2 = 15;
constexpr int PORT_RIGHT_3 = 3;

constexpr int PORT_INTAKE = 7;
constexpr int PORT_INTAKE2 = 2;
constexpr int PORT_OUTTAKE = 8;
constexpr int PORT_COLOR_OPTICAL = 9;

extern pros::MotorGroup left_mg;
extern pros::MotorGroup right_mg;
extern pros::Motor intakeMotor;
extern pros::Motor intakeMotor2;
extern pros::Motor outtakeMotor;
extern pros::Optical colorSensor;

// LemLib Chassis
#include "lemlib/api.hpp"
extern lemlib::Chassis chassis;

#endif
