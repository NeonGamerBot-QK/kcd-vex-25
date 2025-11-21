#ifndef AUTON_MOVEMENT_HPP
#define AUTON_MOVEMENT_HPP

void moveForward(float distance_in, int timeout_ms);
void moveBackward(float distance_in, int timeout_ms);
void turnLeft(int speed, int duration_ms);
void turnRight(int speed, int duration_ms);
void turnTo(float heading_deg, int timeout_ms);
void autonMoveForward();

#endif
