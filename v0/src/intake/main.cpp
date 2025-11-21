#include "main.h"
#include "globals.hpp"
#include "screen/init.hpp"

static int intake_state = 0; // 0=stop, 1=fwd, -1=rev

void intake_task_fn(void* param) {
    colorSensor.set_led_pwm(100); // Enable white light
    while (true) {
        if (intake_state == 1) {
            // Forward
            intakeMotor.move(127);
            
            // Color Sort Logic
            double hue = colorSensor.get_hue();
            int auton = get_selected_auton();
            bool is_red_alliance = (auton == 0 || auton == 1); // Red Left/Right
            bool is_blue_alliance = (auton == 2 || auton == 3); // Blue Left/Right
            
            bool eject = false;
            
            // Hue: Red ~0-20 or 340-360. Blue ~200-240.
            if (is_red_alliance) {
                // Eject Blue
                if (hue > 180 && hue < 260) eject = true; 
            } else if (is_blue_alliance) {
                // Eject Red
                if (hue < 20 || hue > 340) eject = true;
            }
            
            if (eject) {
                // Eject: Reverse second stage, keep first stage forward
                intakeMotor2.move(-127);
                pros::delay(200); // Time to eject
            } else {
                intakeMotor2.move(127);
            }
            
        } else if (intake_state == -1) {
            // Backward
            intakeMotor.move(-127);
            intakeMotor2.move(-127);
        } else {
            // Stop
            intakeMotor.move(0);
            intakeMotor2.move(0);
        }
        pros::delay(10);
    }
}

void intakeForward() {
    static bool task_started = false;
    if (!task_started) {
        pros::Task intakeTask(intake_task_fn, nullptr, "Intake Task");
        task_started = true;
    }
    intake_state = 1;
}

void intakeBackward() {
    // Ensure task is running (if called first)
    static bool task_started = false;
    if (!task_started) {
        pros::Task intakeTask(intake_task_fn, nullptr, "Intake Task");
        task_started = true;
    }
    intake_state = -1;
}

void intakeStop() {
    intake_state = 0;
}
