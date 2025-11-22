#include "main.h"
#include "globals.hpp"
#include "screen/init.hpp"

static int intake_state = 0; // 0=stop, 1=fwd, -1=rev
static bool intake_manual_mode = false; // false=Auto, true=Manual Box (Keep)

const char* getIntakeModeName() {
    return intake_manual_mode ? "Manual Box" : "Auto";
}

void toggleIntakeMode() {
    intake_manual_mode = !intake_manual_mode;
    screen_log("Intake Mode: %s", getIntakeModeName());
}

void intake_task_fn(void* param) {
    colorSensor.set_led_pwm(100); // Enable white light
    uint32_t eject_end_time = 0;
    
    while (true) {
        if (intake_state == 1) {
            // Forward - Bottom motors always run
            intakeMotor.move(-127);
            // intakeMotor3.move(127); // Removed, it's controlled below
            
            bool eject = false;
            
            if (!intake_manual_mode) { // Auto Color Sort
                // Color Sort Logic
                double hue = colorSensor.get_hue();
                int auton = get_selected_auton();
                bool is_red_alliance = (auton == 0 || auton == 1);
                bool is_blue_alliance = (auton == 2 || auton == 3);
                
                bool seen_bad_color = false;
                
                if (is_red_alliance) {
                    if (hue > 180 && hue < 260) seen_bad_color = true; // Blue
                } else if (is_blue_alliance) {
                    if (hue < 20 || hue > 340) seen_bad_color = true; // Red
                }
                
                if (seen_bad_color) {
                    eject_end_time = pros::millis() + 200; // Eject for 200ms
                    screen_log("Sort: Ejecting (%.0f)", hue);
                }
                
                if (pros::millis() < eject_end_time) {
                    eject = true;
                }
            }
            // If intake_manual_mode is true, eject remains false (Keep everything)
            
            if (eject) {
                // Eject: Reverse sorter motor
                intakeMotor3.move(127); 
                intakeMotor2.move(-127); 
            } else {
                intakeMotor3.move(-127);
                intakeMotor2.move(127);
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

void intakeBackward() {
    static bool task_started = false;
    if (!task_started) {
        pros::Task intakeTask(intake_task_fn, nullptr, "Intake Task");
        task_started = true;
    }
    intake_state = 1;
}

void intakeForward() {
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
