#ifndef AUTON_SKILLS_HPP
#define AUTON_SKILLS_HPP

/**
 * @file skills.hpp
 * @brief Skills autonomous routine for 2025-2026 VEX Robotics competition.
 * 
 * The robot completes a sequence of three bridge scoring runs, then parks in the
 * endgame zone. Assumes:
 * - 3 scoring bridges positioned at known locations
 * - Single scoring mechanism (intake/arm)
 * - LemLib chassis controller with odometry
 * - Rotation/encoder tracking for precision
 */

// Autonomous routine main entry point
void autonomousSkills();

// Bridge scoring sequence
void scoreBridge1();
void scoreBridge2();
void scoreBridge3();

// Endgame navigation
void parkInEndgame();

// Utility functions
void scoreOnBridge(float approach_dist, float alignment_turn, float parking_offset);
void approachBridge(float distance);
void alignToBridge();
void depositScore();
void retreatFromBridge();

#endif
