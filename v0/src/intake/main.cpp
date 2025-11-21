#include "main.h"
#include "globals.hpp"
#include "screen/init.hpp"

static int intake_state = 0; // 0=stop, 1=fwd, -1=rev

void intake_task_fn(void* param) {
    colorSensor.set_led_pwm(100); // Enable white light
    uint32_t eject_end_time = 0;
    
    while (true) {
        if (intake_state == 1) {
            // Forward - Bottom motors always run
            intakeMotor.move(127);
            intakeMotor3.move(127);
            
            // Color Sort Logic
            double hue = colorSensor.get_hue();
            int auton = get_selected_auton();
            bool is_red_alliance = (auton == 0 || auton == 1);
            bool is_blue_alliance = (auton == 2 || auton == 3);
            
            bool seen_bad_color = false;
            
            // Hue: Red ~0-20 or 340-360. Blue ~200-240.
            if (is_red_alliance) {
                if (hue > 180 && hue < 260) seen_bad_color = true; // Blue
            } else if (is_blue_alliance) {
                if (hue < 20 || hue > 340) seen_bad_color = true; // Red
            }
            
            if (seen_bad_color) {
                eject_end_time = pros::millis() + 200; // Eject for 200ms
            }
            
            if (pros::millis() < eject_end_time) {
                intakeMotor2.move(-127); // Eject top motor
            } else {
                intakeMotor2.move(127); // Normal top motor
            }
            
        } else if (intake_state == -1) {
            // Backward
            intakeMotor.move(-127);
            intakeMotor2.move(-127);
            intakeMotor3.move(-127);
        } else {
            // Stop
            intakeMotor.move(0);
            intakeMotor2.move(0);
            intakeMotor3.move(0);
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
