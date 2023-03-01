#include "main.h"
#include "autons.hpp"
#include "pros/misc.hpp"
#include "pros/motors.h"

const int DRIVE_SPEED = 110;
const int TURN_SPEED  = 90;
const int SWING_SPEED = 90;
const int MAX_SPEED = 127;

pros::ADIDigitalIn c_switch('E');
pros::ADIDigitalOut expansion('D');
pros::ADIDigitalOut band_release('B');

pros::Motor c(11);
pros::Motor i(10);

Drive chassis (
    // Left
    {1, -2, -3}   

    // Right
    ,{4, -5, 6}

    // IMU Port
    ,9

    // Wheel Diameter
    ,3.25

    // Cartridge RPM
    ,600

    // External Gear Ratio
    ,3.0/5.0
);

void initialize() {
    ez::print_ez_template();
  
    pros::delay(500); // Stop the user from doing anything while legacy ports configure.

    chassis.toggle_modify_curve_with_controller(true);
    chassis.set_active_brake(0.1);
    chassis.set_curve_default(0, 0);

    ez::as::auton_selector.add_autons({
        Auton("SKILLS\nDONT RUN", skills),
        // Auton("basic 3 disk and roller", basic_3_disk),
        Auton("right side 6 disk", right_side),
        Auton("right side 8 disk", right_side_8_disk),
        Auton("Turn to roller\ni hope this is the right one", turn_to_roller),
        Auton("Select an auto", do_nothing),
        Auton("Win Point", wp),
        Auton("Roller and shoot", roller_and_shoot),
        
    });
 
    chassis.initialize();
    ez::as::initialize();

    chassis.set_drive_brake(MOTOR_BRAKE_COAST);

    if (!c_switch.get_value()) {
        //reset cata
        while (1) {
            c.move(-200);
            if (c_switch.get_value()) {
                break;
            }
        }
        c.move(0);
    }
}

void disabled() {}
void competition_initialize() {}

void autonomous() {
    chassis.reset_pid_targets();
    chassis.reset_gyro();
    chassis.reset_drive_sensor();
    chassis.set_drive_brake(MOTOR_BRAKE_HOLD);

    ez::as::auton_selector.call_selected_auton();
}

void opcontrol() {
    c.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    int initial_angle = 0;

    while (true) {
        chassis.tank();

        // if (master.get_digital(DIGITAL_R1)) {
        //     if (master.get_digital_new_press(DIGITAL_R1)) {
        //         initial_angle = chassis.get_gyro();
        //     }

        //     chassis.set_turn_pid(, int speed)
        // }

        bool c_down = c_switch.get_value();

        if (!c_down) {
            c.move(-200);
        }
        else if (master.get_digital(DIGITAL_R2)) {
            c.move(-200);
        } else {
            c.move(0);
        }

        if (master.get_digital(DIGITAL_L1)) {
            i.move(200);
        } else if (master.get_digital(DIGITAL_L2) && c_down) {
            i.move(-200);
        } else {
            i.move(0);
        }

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_B) && master.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)) {
            expansion.set_value(true);
        } else {
            expansion.set_value(false);
        }

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_Y) && master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
            band_release.set_value(true);
        } else {
            band_release.set_value(false);
        }

        pros::delay(ez::util::DELAY_TIME);
    }
}
