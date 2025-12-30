#include "main.h"
#include "auton/movement.hpp"
#include "globals.hpp"
#include <cmath>

// Constants
constexpr float WHEEL_DIAM = 2.75;
constexpr float GEAR_RATIO = 1.0; // Change if geared
constexpr float TICKS_PER_REV = 360.0; // V5 Motor Degrees

void moveForward(float distance_in, int timeout_ms) {
	// Reset encoders
	left_mg.tare_position();
	right_mg.tare_position();
	
	// Constants for 2.75" Omni Wheels, Direct Drive
	// Circumference = 2.75 * pi = 8.639 in
	// 360 ticks / 8.639 in = 41.67 ticks per inch
	float ticks_per_inch = (360.0 / (WHEEL_DIAM * M_PI)) * GEAR_RATIO;
	float target_ticks = distance_in * ticks_per_inch;
	
	// Store initial heading for correction
	float initial_heading = imu.get_heading();
	
	printf("MoveFwd: Dist=%.1f Target=%.1f InitHead=%.1f\n", distance_in, target_ticks, initial_heading);
	
	long start_time = pros::millis();
	
	// P-Loop constants
	float kP = 2.0; // Distance P-gain
	float kP_heading = 3.0; // Heading correction P-gain (tune this)
	
	while (true) {
		// Get average position
		double current_pos = (left_mg.get_position() + right_mg.get_position()) / 2.0;
		
		float error = target_ticks - current_pos;
		
		// Exit if close enough or timeout
		if (std::abs(error) < 10 || (pros::millis() - start_time > timeout_ms)) {
			break;
		}
		
		// Calculate base speed from distance error
		float speed = error * kP;
		
		// Cap speed to max +/- 127
		if (speed > 127) speed = 127;
		if (speed < -127) speed = -127;
		
		// Min speed to overcome friction
		if (std::abs(speed) < 20) speed = (speed > 0) ? 20 : -20;
		
		// Heading correction: keeps robot driving straight
		float current_heading = imu.get_heading();
		float heading_error = initial_heading - current_heading;
		
		// Normalize heading error to [-180, 180]
		if (heading_error > 180) heading_error -= 360;
		if (heading_error < -180) heading_error += 360;
		
		float correction = heading_error * kP_heading;
		
		// Apply correction: if drifting right, slow right side (or speed up left)
		left_mg.move(speed - correction);
		right_mg.move(speed + correction);
		
		pros::delay(10);
	}
	
	// Stop
	left_mg.move(0);
	right_mg.move(0);
	printf("Move Done. Err=%.1f\n", target_ticks - (left_mg.get_position() + right_mg.get_position())/2.0);
}

void moveBackward(float distance_in, int timeout_ms) {
	moveForward(-distance_in, timeout_ms);
}

void turnTo(float heading_deg, int timeout_ms) {
	// Placeholder or IMU turn implementation
	// chassis.turnToHeading(heading_deg, timeout_ms); 
	// Disabled chassis use
}

// Legacy wrappers
void turnLeft(int speed, int duration_ms) {
	left_mg.move_velocity(speed * 4.5);
	right_mg.move_velocity(-speed * 4.5);
	pros::delay(duration_ms);
	left_mg.move_velocity(0);
	right_mg.move_velocity(0);
}

void turnRight(int speed, int duration_ms) {
	left_mg.move_velocity(-speed * 4.5);
	right_mg.move_velocity(speed * 4.5);
	pros::delay(duration_ms);
	left_mg.move_velocity(0);
	right_mg.move_velocity(0);
}

void turnByDegrees(float degrees, int timeout_ms) {
	// Approximately 4.5ms per degree for 90 degree turn
	// 90 deg / 1000ms = 0.09 deg/ms, so 1 deg ≈ 11.1ms
	int duration_ms = std::abs(degrees) * 11.1;
	if (duration_ms > timeout_ms) duration_ms = timeout_ms;
	
	if (degrees > 0) {
		// Turn right
		turnRight(90, duration_ms);
	} else {
		// Turn left
		turnLeft(90, duration_ms);
	}
}

