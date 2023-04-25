#include "autons.hpp"
#include "EZ-Template/util.hpp"
#include "main.h"
#include "pros/adi.hpp"
#include "pros/motors.h"
#include "pros/rtos.h"
#include "pros/rtos.hpp"

const int DRIVE_SPEED = 110;
const int TURN_SPEED  = 80;
const int SWING_SPEED = 80;

const int MAX_SPEED = 127;

pros::ADIDigitalIn cata_switch('E');
pros::ADIDigitalOut e('G');

pros::Motor intake(10);
pros::Motor cata(11);
pros::ADIDigitalOut b_rl('B');
pros::ADIDigitalOut ex('D');
pros::ADIDigitalOut intake_piston('G');



void default_constants() {
  chassis.set_slew_min_power(10, 10);
  chassis.set_slew_distance(50, 50);
  chassis.set_pid_constants(&chassis.headingPID, 8, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

void shoot() {
    cata.move_relative(-200, 200);
    pros::delay(10);
}

void reload() {
    if (intake.get_direction() == -1) {
        intake.move(0);
    }

    while (1) {
        if (cata_switch.get_value() == 1) {
            cata.move(0);
            break;
        } else {
            cata.move(-200);
        }
    }
    pros::delay(10);
}

void sr(bool restart_intake, bool override_pause) {
    if (!override_pause) {
        intake.move(0);
    }
    
    cata.move(-200);
    pros::delay(200);

    while (1) {
        intake.move(0);
        if (cata_switch.get_value()) {
            break;
        }
    }

    if (restart_intake && !override_pause) {
        intake.move(-200);
    }

    cata.move(0);
}

//shoot then reload asyncronously
void sr_async(bool restart_intake) {
    pros::Task task{[=] {
        sr(restart_intake, false);
        pros::delay(10);
    }};

    pros::delay(300);
}

//shoot then reload asyncronously
void sr_async_no_override(bool restart_intake) {
    pros::Task task{[=] {
        sr(restart_intake, true);
        pros::delay(10);
    }};

    pros::delay(300);
}

void right_side_6_disk () {

}

void basic_3_disk() {
    intake.move(-200);

    chassis.set_drive_pid(110, 80);
    chassis.wait_drive();

    chassis.set_turn_pid(-135, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-30, MAX_SPEED);
    chassis.wait_until(-10);

    sr_async(true);

    chassis.set_turn_pid(-90, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(120, DRIVE_SPEED);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::LEFT_SWING, -160, 40);
    chassis.wait_until(-170);

    intake.move_relative(300, 200);
}

void right_side() {
    //start intake
    intake.move(-200);

    //drive forward to pick up first disk (+ 2 preloads)
    chassis.set_drive_pid(110, 80);
    chassis.wait_drive();

    //turn to correct shooting angle for first 3 shot
    chassis.set_turn_pid(-135, TURN_SPEED);
    chassis.wait_drive();

    sr_async(true);

    //drive back and face 3 disk line
    chassis.set_turn_pid(-225, TURN_SPEED);
    chassis.wait_drive();

    //pick up 3 middle disks
    chassis.set_drive_pid(70, 50);
    chassis.wait_drive();

    //turn and shoot
    //-289

    chassis.set_turn_pid(-150, TURN_SPEED);
    chassis.wait_drive();

    intake.move(0);

    sr_async(true);

    chassis.set_swing_pid(ez::RIGHT_SWING, -260, SWING_SPEED);
    chassis.wait_drive();

    intake_piston.set_value(true);

    chassis.set_drive_pid(55, 70);
    chassis.wait_until(50);

    intake_piston.set_value(false);

    chassis.set_drive_pid(25, 15);
    chassis.wait_drive();

    chassis.set_turn_pid(-170, TURN_SPEED);
    chassis.wait_drive();

    sr_async(false);

    chassis.set_turn_pid(-180, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(60, DRIVE_SPEED);

    chassis.set_swing_pid(ez::RIGHT_SWING, -190, SWING_SPEED);
    intake.move_relative(400, 200);
    chassis.wait_drive();


}


void right_side_8_disk() {
    // intake first disk
    
    intake.move(-200);

    chassis.set_drive_pid(110, DRIVE_SPEED);
    chassis.wait_drive();

    chassis.set_turn_pid(-135, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-30, MAX_SPEED);
    chassis.wait_until(-10);

    // shoot first disk + preloads
    sr_async(true);

    chassis.set_drive_pid(32, 80, true);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::RIGHT_SWING, -175, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(70, 60);
    chassis.wait_drive();

    chassis.set_drive_pid(-70, MAX_SPEED);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::RIGHT_SWING, -135, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-50, MAX_SPEED);
    chassis.wait_until(-10);

    // shoot 3 low goal disks
    sr_async(true);

    chassis.set_drive_pid(30, MAX_SPEED, true);
    chassis.wait_drive();

    chassis.set_turn_pid(-225, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(60, 80);
    chassis.wait_until(58);

    chassis.set_drive_pid(1500, MAX_SPEED);

    chassis.set_turn_pid(-170, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-30, MAX_SPEED);
    chassis.wait_until(-10);

    intake.move(0);

    sr_async(false);

    chassis.wait_drive();

    chassis.set_drive_pid(20, MAX_SPEED);
    chassis.wait_drive();

    chassis.set_turn_pid(-230, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(80, MAX_SPEED);
    chassis.wait_drive();

    intake.move_relative(300, 200);
}

void roller_and_shoot() {
    chassis.set_drive_pid(5, MAX_SPEED);

    intake.move_relative(400, 200);
    pros::delay(500);

    chassis.set_drive_pid(-50, 50, true);
    chassis.wait_drive();

    chassis.set_turn_pid(-15, 40);
    chassis.wait_drive();

    sr_async(false);

    b_rl.set_value(true);
    pros::delay(500);
    b_rl.set_value(false);
}

void left_side_5_disk() {
    chassis.set_swing_pid(ez::RIGHT_SWING, -10, 30);
    pros::delay(300);
    
    intake.move_relative(400, 200);
    pros::delay(300);

    chassis.set_swing_pid(ez::RIGHT_SWING, 0, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-45, 50, true);
    chassis.wait_drive();

    chassis.set_turn_pid(-10, TURN_SPEED);
    chassis.wait_drive();

    sr_async(true);
    chassis.wait_drive();

    chassis.set_drive_pid(10, DRIVE_SPEED);

    chassis.set_turn_pid(-105, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(65, 80);
    chassis.wait_drive();

    chassis.set_drive_pid(50, 30);
    chassis.wait_drive();

    chassis.set_turn_pid(-37, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-55, MAX_SPEED, true);
    chassis.wait_until(-45);

    sr_async(true);
}

void left_side_8_disk() {
    chassis.set_swing_pid(ez::RIGHT_SWING, -10, 30);
    pros::delay(300);
    
    intake.move_relative(400, 200);
    pros::delay(300);

    chassis.set_swing_pid(ez::RIGHT_SWING, 0, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-43, 50, true);
    chassis.wait_drive();

    chassis.set_turn_pid(-10, TURN_SPEED);
    chassis.wait_drive();

    sr_async(true);

    chassis.set_drive_pid(15, DRIVE_SPEED);
    chassis.wait_drive();

    chassis.set_turn_pid(165, TURN_SPEED);
    chassis.wait_drive();

    intake_piston.set_value(true);

    chassis.set_drive_pid(18, 70);
    chassis.wait_drive();

    intake_piston.set_value(false);

    pros::delay(2000);

    chassis.set_drive_pid(-20, DRIVE_SPEED);
    chassis.wait_drive();

    chassis.set_turn_pid(-10, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-25, MAX_SPEED, true);
    chassis.wait_until(-20);
    sr_async(true);
    chassis.wait_drive();

    chassis.set_turn_pid(-105, TURN_SPEED);
    chassis.wait_drive();

    intake_piston.set_value(true);

    chassis.set_drive_pid(48, DRIVE_SPEED);
    chassis.wait_drive();

    pros::delay(200);

    intake_piston.set_value(false);
    pros::delay(1000);

    chassis.set_turn_pid(-23, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-20, MAX_SPEED, true);
    chassis.wait_until(-15);
    sr_async(false);
    chassis.wait_drive();
}

void wp() {
    chassis.set_swing_pid(ez::RIGHT_SWING, -10, 30);
    pros::delay(300);
    
    intake.move_relative(400, 200);
    pros::delay(300);

    chassis.set_swing_pid(ez::RIGHT_SWING, 0, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-45, 50, true);
    chassis.wait_drive();

    chassis.set_turn_pid(-12, TURN_SPEED);
    chassis.wait_drive();

    sr_async(true);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::RIGHT_SWING, -135, SWING_SPEED);
    chassis.wait_drive();

    intake_piston.set_value(true);

    chassis.set_drive_pid(22, MAX_SPEED);
    chassis.wait_drive();

    intake_piston.set_value(false);

    intake.move(-200);

    chassis.set_drive_pid(20, 30);
    chassis.wait_drive();

    chassis.set_drive_pid(40, DRIVE_SPEED, true);
    chassis.wait_drive();

    chassis.set_turn_pid(-30, 45);
    chassis.wait_drive();

    sr_async(true);

    chassis.set_turn_pid(-135, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(200, 70);
    chassis.wait_drive();

    chassis.set_turn_pid(-80, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-25, MAX_SPEED, true);
    chassis.wait_until(-20);
    sr_async_no_override(false);
    chassis.wait_drive();

    chassis.set_turn_pid(-127, 45);
    chassis.wait_drive();

    chassis.set_drive_pid(80, MAX_SPEED);
    chassis.wait_until(70);

    pros::delay(200);

    intake.move_relative(400, 200);
    
};

void turn_to_roller() {
    chassis.set_drive_pid(60, DRIVE_SPEED);
    chassis.wait_drive();

    chassis.set_turn_pid(90, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(30, DRIVE_SPEED);
    chassis.wait_until(25);

    intake.move_relative(-400, 200);
}

// chassis.set_turn_pid(-135, 50);
// chassis.wait_drive();

// chassis.set_drive_pid(150, 50);

// chassis.set_turn_pid(-60, 40);
// chassis.wait_drive();

// chassis.set_drive_pid(-20, 80);
// chassis.wait_drive();

// sr_async();
// pros::delay(500);

// chassis.set_turn_pid(-135, 50);
// chassis.wait_drive();

// chassis.set_drive_pid(50, 80);
// chassis.wait_drive();

// chassis.set_drive_pid(20, DRIVE_SPEED);
// chassis.wait_until(15);

// intake.move_relative(400, 200);

void skills() {
    chassis.set_swing_pid(ez::RIGHT_SWING, -10, 30);
    pros::delay(300);
    
    intake.move_relative(500, 200);
    pros::delay(500);

    chassis.set_swing_pid(ez::RIGHT_SWING, 0, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-7, DRIVE_SPEED);
    chassis.wait_drive();

    intake.move(-200);
    
    chassis.set_turn_pid(130, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(63, 60);
    chassis.wait_drive();

    intake.move(0);

    chassis.set_swing_pid(ez::RIGHT_SWING, 80, SWING_SPEED);
    chassis.wait_until(85);

    intake.move_relative(600, 200);
    pros::delay(800);

    chassis.set_swing_pid(ez::LEFT_SWING, 1, SWING_SPEED);
    chassis.wait_drive();

    intake.move(-100);
    
    chassis.set_drive_pid(-130, MAX_SPEED);
    chassis.wait_drive();

    sr_async(true);

    chassis.set_swing_pid(ez::RIGHT_SWING, -45, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(53, 40);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::RIGHT_SWING, -135, 60);
    chassis.wait_drive();

    chassis.set_drive_pid(78, 60, true);
    chassis.wait_drive();

    chassis.set_turn_pid(-45, TURN_SPEED);
    chassis.wait_drive();

    sr_async(true);

    chassis.set_turn_pid(0, 50);
    chassis.wait_drive();

    chassis.set_drive_pid(50, 40);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::LEFT_SWING, 45, 60);
    chassis.wait_drive();

    chassis.set_drive_pid(35, 40);
    chassis.wait_drive();

    chassis.set_turn_pid(-21, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-40, MAX_SPEED);
    chassis.wait_until(-30);

    sr_async(true);

    chassis.set_turn_pid(-25, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(100, 30);
    chassis.wait_drive();

    chassis.set_turn_pid(92, TURN_SPEED);
    chassis.wait_drive();

    sr_async(true);

    chassis.set_turn_pid(210, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(115, 70);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::LEFT_SWING, 225, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(80, 60, true);
    chassis.wait_drive();

    chassis.set_turn_pid(160, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-23, DRIVE_SPEED);
    chassis.wait_drive();

    sr_async(true);

    chassis.set_turn_pid(145, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(60, 20);
    chassis.wait_drive();

    chassis.set_turn_pid(45, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(50, 50);
    chassis.wait_drive();

    chassis.set_turn_pid(140, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-20, MAX_SPEED);
    chassis.wait_until(-10);

    sr_async(true);

    chassis.wait_drive();

    chassis.set_turn_pid(180, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(110, 30);
    chassis.wait_drive();

    chassis.set_turn_pid(270, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-20, DRIVE_SPEED);
    chassis.wait_drive();

    sr_async(false);

    chassis.set_drive_pid(80, MAX_SPEED);
    chassis.wait_drive();

    chassis.set_turn_pid(180, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(35, DRIVE_SPEED);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::RIGHT_SWING, 170, SWING_SPEED);
    chassis.wait_drive();

    intake.move_relative(500, 200);
    pros::delay(500);

    chassis.set_swing_pid(ez::RIGHT_SWING, 180, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(-7, DRIVE_SPEED);
    chassis.wait_drive();

    intake.move(-200);
    
    chassis.set_turn_pid(310, TURN_SPEED);
    chassis.wait_drive();

    chassis.set_drive_pid(80, 60);
    chassis.wait_drive();

    intake.move(0);

    chassis.set_swing_pid(ez::RIGHT_SWING, 260, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::RIGHT_SWING, 260, SWING_SPEED);
    chassis.wait_drive();

    chassis.set_swing_pid(ez::LEFT_SWING, 270, SWING_SPEED);
    chassis.wait_drive();

    intake.move_relative(600, 200);
    pros::delay(800);

    chassis.set_drive_pid(-50, DRIVE_SPEED);
    chassis.wait_drive();

    chassis.set_turn_pid(180, TURN_SPEED);
    chassis.wait_drive();

    sr_async(false);

    chassis.set_turn_pid(225, DRIVE_SPEED);
    chassis.wait_drive();

    ex.set_value(false);
}

void do_nothing() {}


// void skills() {
//     chassis.set_swing_pid(ez::RIGHT_SWING, -10, 30);
//     pros::delay(300);
    
//     intake.move_relative(500, 200);
//     pros::delay(500);

//     chassis.set_swing_pid(ez::RIGHT_SWING, 0, SWING_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(-7, DRIVE_SPEED);
//     chassis.wait_drive();

//     intake.move(-200);
    
//     chassis.set_turn_pid(130, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(63, 60);
//     chassis.wait_drive();

//     intake.move(0);

//     chassis.set_swing_pid(ez::RIGHT_SWING, 80, SWING_SPEED);
//     chassis.wait_until(85);

//     intake.move_relative(600, 200);
//     pros::delay(800);

//     chassis.set_swing_pid(ez::LEFT_SWING, 1, SWING_SPEED);
//     chassis.wait_drive();

//     intake.move(-100);
    
//     chassis.set_drive_pid(-130, MAX_SPEED);
//     chassis.wait_drive();

//     sr_async(true);

//     chassis.set_swing_pid(ez::RIGHT_SWING, -45, SWING_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(53, 40);
//     chassis.wait_drive();

//     chassis.set_swing_pid(ez::RIGHT_SWING, -135, 60);
//     chassis.wait_drive();

//     chassis.set_drive_pid(78, 60, true);
//     chassis.wait_drive();

//     chassis.set_turn_pid(-45, TURN_SPEED);
//     chassis.wait_drive();

//     sr_async(true);

//     chassis.set_turn_pid(0, 50);
//     chassis.wait_drive();

//     chassis.set_drive_pid(50, 40);
//     chassis.wait_drive();

//     chassis.set_swing_pid(ez::LEFT_SWING, 45, 60);
//     chassis.wait_drive();

//     chassis.set_drive_pid(35, 40);
//     chassis.wait_drive();

//     chassis.set_turn_pid(-21, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(-40, MAX_SPEED);
//     chassis.wait_until(-30);

//     sr_async(true);

//     chassis.set_turn_pid(-25, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(100, 30);
//     chassis.wait_drive();

//     chassis.set_turn_pid(92, TURN_SPEED);
//     chassis.wait_drive();

//     sr_async(true);

//     chassis.set_turn_pid(210, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(115, 70);
//     chassis.wait_drive();

//     chassis.set_swing_pid(ez::LEFT_SWING, 225, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(80, 60, true);
//     chassis.wait_drive();

//     chassis.set_turn_pid(160, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(-23, DRIVE_SPEED);
//     chassis.wait_drive();

//     sr_async(true);

//     chassis.set_turn_pid(145, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(60, 20);
//     chassis.wait_drive();

//     chassis.set_turn_pid(45, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(50, 50);
//     chassis.wait_drive();

//     chassis.set_turn_pid(140, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(-20, MAX_SPEED);
//     chassis.wait_until(-10);

//     sr_async(true);

//     chassis.wait_drive();

//     chassis.set_turn_pid(180, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(110, 30);
//     chassis.wait_drive();

//     chassis.set_turn_pid(270, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(-20, DRIVE_SPEED);
//     chassis.wait_drive();

//     sr_async(false);

//     chassis.set_drive_pid(80, MAX_SPEED);
//     chassis.wait_drive();

//     chassis.set_turn_pid(180, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(35, DRIVE_SPEED);
//     chassis.wait_drive();

//     chassis.set_swing_pid(ez::RIGHT_SWING, 170, SWING_SPEED);
//     chassis.wait_drive();

//     intake.move_relative(500, 200);
//     pros::delay(500);

//     chassis.set_swing_pid(ez::RIGHT_SWING, 180, SWING_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(-7, DRIVE_SPEED);
//     chassis.wait_drive();

//     intake.move(-200);
    
//     chassis.set_turn_pid(310, TURN_SPEED);
//     chassis.wait_drive();

//     chassis.set_drive_pid(80, 60);
//     chassis.wait_drive();

//     intake.move(0);

//     chassis.set_swing_pid(ez::RIGHT_SWING, 260, SWING_SPEED);
//     chassis.wait_drive();

//     chassis.set_swing_pid(ez::RIGHT_SWING, 260, SWING_SPEED);
//     chassis.wait_drive();

//     chassis.set_swing_pid(ez::LEFT_SWING, 270, SWING_SPEED);
//     chassis.wait_drive();

//     intake.move_relative(600, 200);
//     pros::delay(800);

//     chassis.set_drive_pid(-50, DRIVE_SPEED);
//     chassis.wait_drive();

//     chassis.set_turn_pid(180, TURN_SPEED);
//     chassis.wait_drive();

//     sr_async(false);

//     chassis.set_turn_pid(225, DRIVE_SPEED);
//     chassis.wait_drive();

//     ex.set_value(true);
// }