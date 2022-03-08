#include "main.h"
#include "autons.hpp"

pros::ADIDigitalOut lift_grab('A');
pros::ADIDigitalOut forklift_grab('B');

pros::Motor forklift(10);
pros::Motor lift(2);

Drive chassis (
  // Left
  {-11, -1, -12}

  // Right
  ,{4, 20, 6}

  // IMU Port
  ,8

  // Wheel Diameter
  ,3.25

  // Cartridge RPM
  ,200

  // External Gear Ratio
  ,0.5
);

void initialize() {
  lift_grab.set_value(true);
  ez::print_ez_template();

  pros::delay(500);

  chassis.toggle_modify_curve_with_controller(true);
  chassis.set_active_brake(0.1);
  chassis.set_curve_default(0, 0);

  ez::as::auton_selector.add_autons({
    Auton("Right wp\nGrab 2 neutral mogos and score 1 ally ring", right_2_mogo_wp),
    Auton("Grab Mogo\n\nGrab one mogo and come back", grab_mogo),
    Auton("2 right mogo\n\nGrab 2 neutral mogos (right side)", right_2_mogo),
    Auton("do nothing\n\njust sit there and be a lazy f*ck (if other team has wp auto)", do_nothing),
  });

  chassis.initialize();
  ez::as::initialize();

  chassis.set_drive_brake(MOTOR_BRAKE_COAST);
  lift.set_brake_mode(MOTOR_BRAKE_HOLD);
	forklift.set_brake_mode(MOTOR_BRAKE_HOLD);
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

bool lift_grab_toggle = false;
bool forklift_grab_toggle = false;

void opcontrol() {

  while (true) {
    chassis.tank();

    if (master.get_digital(DIGITAL_L2)) {
		lift.move_voltage(12000);
		} else if (master.get_digital(DIGITAL_L1)) {
			lift.move_voltage(-12000);
		} else {
			lift.move(0);
		}

		if (master.get_digital(DIGITAL_X)) {
			forklift.move_voltage(12000);
		} else if (master.get_digital(DIGITAL_A)) {
			forklift.move_voltage(-12000);
		} else {
			forklift.move(0);
		}

    if (master.get_digital_new_press(DIGITAL_R2)) {
	    if (!lift_grab_toggle) {
	        lift_grab.set_value(true);
	        lift_grab_toggle = !lift_grab_toggle;
	    }
	    else {
	        lift_grab.set_value(false);
	        lift_grab_toggle = !lift_grab_toggle;
	    }
		}

		if (master.get_digital_new_press(DIGITAL_R1)) {
	    if (!forklift_grab_toggle) {
	        forklift_grab.set_value(true);
	        forklift_grab_toggle = !forklift_grab_toggle;
	    }
	    else {
	        forklift_grab.set_value(false);
	        forklift_grab_toggle = !forklift_grab_toggle;
	    }
		}

    pros::delay(ez::util::DELAY_TIME);
  }
}

