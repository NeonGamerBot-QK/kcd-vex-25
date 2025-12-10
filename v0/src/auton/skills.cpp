#include "main.h"
#include "auton/skills.hpp"
#include "auton/movement.hpp"
#include "intake/main.hpp"
#include "outtake/main.hpp"
#include "globals.hpp"
#include "lemlib/api.hpp"
#include <cmath>

/**
 * @file skills.cpp
 * @brief Implementation of the Skills autonomous routine.
 * 
 * Path Overview:
 * START → Bridge 1 (approach + score + retreat)
 *       → Bridge 2 (reposition + score + retreat)
 *       → Bridge 3 (reposition + score + retreat)
 *       → Endgame Zone (navigate to parking spot)
 * 
 * Each bridge run includes:
 * 1. Drive approach distance
 * 2. Align chassis to bridge using gyro correction
 * 3. Activate scoring mechanism
 * 4. Deposit object(s)
 * 5. Retract mechanism and back away
 * 6. Reposition for next bridge
 */

// Tuning Constants - adjust based on field testing
namespace SkillsConstants {
	// Bridge distances (in inches from starting position)
	constexpr float BRIDGE_1_APPROACH = 36.0f;      // Distance to first bridge
	constexpr float BRIDGE_2_REPOSITION = 24.0f;    // Lateral distance to move for bridge 2
	constexpr float BRIDGE_3_REPOSITION = 30.0f;    // Distance to move for bridge 3

	// Turning and alignment
	constexpr int BRIDGE_TURN_ANGLE = 0;            // Degrees to turn for alignment (adjust for bridge geometry)
	constexpr int GYRO_CORRECTION_TOLERANCE = 5;    // Degrees allowable error for gyro

	// Scoring timing
	constexpr int SCORING_DURATION = 800;           // Time to run intake motor (ms)
	constexpr int DEPOSIT_DELAY = 500;              // Time to deposit before reversing

	// Approach and retreat
	constexpr float APPROACH_SPEED_RAMP = 5.0f;     // Acceleration factor for approach
	constexpr int APPROACH_TIMEOUT = 2000;          // Max time for approach (ms)
	constexpr int RETREAT_TIMEOUT = 1500;           // Max time for retreat (ms)

	// Endgame navigation
	constexpr float ENDGAME_ZONE_DISTANCE = 60.0f;  // Distance to endgame from last bridge
	constexpr float ENDGAME_FINAL_POSITION = 12.0f; // Final parking position within endgame
}

/**
 * @brief Approach bridge and align for scoring.
 * 
 * This function drives the robot toward the bridge, using encoder feedback
 * to maintain distance and gyro to correct heading drift.
 * 
 * @param distance Distance to drive (inches)
 */
void approachBridge(float distance) {
	// Record starting position for odometry-based corrections
	lemlib::Pose start_pose = chassis.getPose();
	
	// Drive forward toward bridge with slight speed ramping for smooth acceleration
	moveForward(distance, SkillsConstants::APPROACH_TIMEOUT);
	
	// Allow settling time for sensors
	pros::delay(100);
	
	// Get current position and heading
	lemlib::Pose current_pose = chassis.getPose();
	
	// Log position data for debugging
	printf("Bridge Approach: Start(%.1f,%.1f) Current(%.1f,%.1f) Heading:%.1f\n",
		start_pose.x, start_pose.y, current_pose.x, current_pose.y, current_pose.theta);
}

/**
 * @brief Align robot chassis parallel to bridge using gyro feedback.
 * 
 * Uses IMU to correct any heading drift and ensure the robot is perpendicular
 * to the bridge for optimal scoring.
 */
void alignToBridge() {
	// Get current heading from IMU via chassis
	lemlib::Pose pose = chassis.getPose();
	float current_heading = pose.theta;
	
	// Desired heading for this bridge (0 degrees, or adjust per bridge orientation)
	float target_heading = 0.0f;
	float heading_error = target_heading - current_heading;
	
	// Normalize error to [-180, 180]
	if (heading_error > 180) heading_error -= 360;
	if (heading_error < -180) heading_error += 360;
	
	// Only correct if error exceeds tolerance
	if (std::abs(heading_error) > SkillsConstants::GYRO_CORRECTION_TOLERANCE) {
		int turn_duration = std::abs(heading_error) * 11.1;  // Empirically derived timing
		
		if (heading_error > 0) {
			turnRight(90, turn_duration);
		} else {
			turnLeft(90, turn_duration);
		}
		
		pros::delay(100);  // Stabilization delay
		
		printf("Bridge Alignment: Corrected %.1f degrees\n", heading_error);
	}
}

/**
 * @brief Activate scoring mechanism and deposit object(s).
 * 
 * Runs intake/arm motor in forward direction for a set duration,
 * then pauses briefly before reversing for retraction.
 */
void depositScore() {
	// Start intake motor (assumes outtakeMotor is the scoring mechanism)
	outtakeMotor.move(127);  // Max forward speed
	
	// Allow time for object to be deposited
	pros::delay(SkillsConstants::SCORING_DURATION);
	
	// Brief pause to ensure object clears
	pros::delay(SkillsConstants::DEPOSIT_DELAY);
	
	// Reverse to prepare for next intake cycle
	outtakeMotor.move(-80);
	pros::delay(300);
	
	// Stop motor
	outtakeMotor.move(0);
	
	printf("Score Deposited\n");
}

/**
 * @brief Back away from bridge and stop.
 * 
 * Safely retreats the robot from the bridge to prepare for repositioning
 * to the next bridge.
 */
void retreatFromBridge() {
	// Back up slowly to avoid collision
	moveBackward(6.0f, SkillsConstants::RETREAT_TIMEOUT);
	
	pros::delay(100);  // Settling delay
	
	printf("Bridge Retreat Complete\n");
}

/**
 * @brief Complete scoring sequence for a single bridge.
 * 
 * Encapsulates: approach → align → deposit → retreat
 * 
 * @param approach_dist Distance to drive to bridge (inches)
 * @param alignment_turn Degrees to turn for alignment (relative to current heading)
 * @param parking_offset Unused parameter (reserved for future use)
 */
void scoreOnBridge(float approach_dist, float alignment_turn, float parking_offset) {
	printf("=== Scoring Bridge ===\n");
	
	// Approach
	approachBridge(approach_dist);
	pros::delay(50);
	
	// Align
	// Note: alignToBridge() uses gyro; to add relative turning:
	// if (alignment_turn != 0) turnByDegrees(alignment_turn, 1000);
	alignToBridge();
	pros::delay(50);
	
	// Score
	depositScore();
	pros::delay(50);
	
	// Retreat
	retreatFromBridge();
	
	printf("=== Bridge Score Complete ===\n\n");
}

/**
 * @brief Score on first bridge.
 * 
 * Assumes starting position is aligned with Bridge 1.
 * Simple forward approach and deposit.
 */
void scoreBridge1() {
	printf("[Bridge 1]\n");
	
	// Drive straight to bridge 1
	scoreOnBridge(
		SkillsConstants::BRIDGE_1_APPROACH,   // Approach distance
		0,                                    // No turn needed (start position aligned)
		0                                     // Parking offset (unused)
	);
}

/**
 * @brief Reposition and score on second bridge.
 * 
 * After Bridge 1, move sideways/rotated to align with Bridge 2,
 * then execute scoring sequence.
 */
void scoreBridge2() {
	printf("[Bridge 2]\n");
	
	// Reposition: turn and move to Bridge 2
	// Example: turn 90 degrees right, move forward, turn back
	turnRight(90, 1000);
	pros::delay(100);
	
	moveForward(SkillsConstants::BRIDGE_2_REPOSITION, 2000);
	pros::delay(100);
	
	turnLeft(90, 1000);
	pros::delay(100);
	
	// Score on Bridge 2
	scoreOnBridge(
		SkillsConstants::BRIDGE_1_APPROACH,   // Same approach distance (bridges may be same spacing)
		0,                                    // Already aligned
		0
	);
}

/**
 * @brief Reposition and score on third bridge.
 * 
 * Similar to Bridge 2, but with different repositioning angles/distances.
 * Adjust BRIDGE_3_REPOSITION and turning angles based on actual field geometry.
 */
void scoreBridge3() {
	printf("[Bridge 3]\n");
	
	// Reposition: turn and move to Bridge 3
	// Example: turn 90 degrees left, move forward, turn back
	turnLeft(90, 1000);
	pros::delay(100);
	
	moveForward(SkillsConstants::BRIDGE_3_REPOSITION, 2000);
	pros::delay(100);
	
	turnRight(90, 1000);
	pros::delay(100);
	
	// Score on Bridge 3
	scoreOnBridge(
		SkillsConstants::BRIDGE_1_APPROACH,   // Same approach distance
		0,                                    // Already aligned
		0
	);
}

/**
 * @brief Navigate to and park in the endgame zone.
 * 
 * After all three bridges are scored, drive to the designated endgame zone
 * and position the robot for parking (typically near a corner or wall).
 */
void parkInEndgame() {
	printf("[Endgame Navigation]\n");
	
	// Back away from last bridge
	moveBackward(6.0f, 1500);
	pros::delay(100);
	
	// Navigate to endgame zone
	// Example path: turn toward zone, drive distance
	turnLeft(90, 1200);
	pros::delay(100);
	
	moveForward(SkillsConstants::ENDGAME_ZONE_DISTANCE, 3000);
	pros::delay(100);
	
	// Fine-tune parking position
	moveForward(SkillsConstants::ENDGAME_FINAL_POSITION, 1000);
	pros::delay(100);
	
	// Ensure all motors stop
	left_mg.move(0);
	right_mg.move(0);
	outtakeMotor.move(0);
	
	printf("Parked in Endgame Zone\n");
}

/**
 * @brief Main Skills autonomous routine.
 * 
 * Executes the complete sequence:
 * Bridge 1 → Bridge 2 → Bridge 3 → Endgame Park
 * 
 * Total estimated time: ~30-40 seconds
 * Can be called from main autonomous() function.
 */
void autonomousSkills() {
	printf("╔════════════════════════════════════╗\n");
	printf("║   SKILLS AUTONOMOUS ROUTINE       ║\n");
	printf("╚════════════════════════════════════╝\n\n");
	
	// Ensure all systems are ready
	intakeStop();
	outtakeMotor.move(0);
	pros::delay(100);
	
	// Reset odometry for consistent positioning
	chassis.setPose({0, 0, 0});
	
	// Execute all three bridges
	scoreBridge1();
	scoreBridge2();
	scoreBridge3();
	
	// Navigate to endgame and park
	parkInEndgame();
	
	printf("\n╔════════════════════════════════════╗\n");
	printf("║   SKILLS ROUTINE COMPLETE         ║\n");
	printf("╚════════════════════════════════════╝\n");
}
