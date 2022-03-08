#include "autons.hpp"
#include "EZ-Template/util.hpp"
#include "main.h"

const int DRIVE_SPEED = 110;
const int TURN_SPEED  = 90;
const int SWING_SPEED = 90;

const int MAX_SPEED = 127;

pros::ADIDigitalOut l_grab('A');
pros::ADIDigitalOut f_grab('B');
pros::Motor l(2);
pros::Motor f(10);

void default_constants() {
  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 8, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

void tug (int attempts) {
  for (int i=0; i<attempts-1; i++) {
    // Attempt to drive backwards
    printf("i - %i", i);
    chassis.set_drive_pid(-12, 127);
    l.move_absolute(0, 200);
    chassis.wait_drive();

    // If failsafed...
    if (chassis.interfered) {
      chassis.reset_drive_sensor();
      chassis.set_drive_pid(-2, 20);
      l.move_absolute(0, 200);
      pros::delay(1000);
    }
    // If robot successfully drove back, return
    else {
      l.move_absolute(0, 200);
      return;
    }
  }
}

void grab_mogo () {
  l_grab.set_value(false);
  f_grab.set_value(false);

  chassis.set_drive_pid(42, 127, false);
  
  chassis.wait_until(39);
  l_grab.set_value(true);

  
  chassis.set_drive_pid(-45, 127, false);
  chassis.wait_drive();

  if (chassis.interfered) {
   l.move_voltage(75);
   tug(15);
   return;
 }
}


void right_2_mogo () {
  l_grab.set_value(false);
  f_grab.set_value(false);

  chassis.set_drive_pid(40, MAX_SPEED, false);
  
  chassis.wait_until(37);
  l_grab.set_value(true);

  
  chassis.set_drive_pid(-10, MAX_SPEED, false);
  chassis.wait_drive();

  if (chassis.interfered) {
   l.move_absolute(0, 200);
   tug(15);
   
   chassis.set_drive_pid(-30, MAX_SPEED, false);
   chassis.wait_drive();
   
   return;
 }

  chassis.set_swing_pid(ez::RIGHT_SWING, 139, MAX_SPEED);
  chassis.wait_until(136);

  chassis.set_drive_pid(-30, DRIVE_SPEED, false);
  chassis.wait_until(-17);
  chassis.set_max_speed(30);
  chassis.wait_drive();
  
  f_grab.set_value(true);

  chassis.set_drive_pid(56, MAX_SPEED, true);
  chassis.wait_until(54);

  chassis.set_turn_pid(-45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-20, MAX_SPEED, false);
  chassis.wait_drive();
  
  f_grab.set_value(false);

  chassis.set_drive_pid(27, MAX_SPEED, false);
  chassis.wait_until(15);
  f.move_absolute(-3800, 200);
  chassis.wait_drive();

  chassis.set_turn_pid(-90, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-20, 75, false);
  chassis.wait_drive();
}

void right_2_mogo_wp () {
  l_grab.set_value(false);
  f_grab.set_value(false);

  chassis.set_drive_pid(40, MAX_SPEED, false);
  
  chassis.wait_until(37);
  l_grab.set_value(true);

  
  chassis.set_drive_pid(-10, MAX_SPEED, false);
  chassis.wait_drive();

  if (chassis.interfered) {
   l.move_absolute(0, 200);
   tug(15);
   
   chassis.set_drive_pid(-30, MAX_SPEED, false);
   chassis.wait_drive();
   
   return;
 }

  chassis.set_swing_pid(ez::RIGHT_SWING, 139, MAX_SPEED);
  chassis.wait_until(136);

  chassis.set_drive_pid(-30, DRIVE_SPEED, false);
  chassis.wait_until(-17);
  chassis.set_max_speed(30);
  chassis.wait_drive();
  
  f_grab.set_value(true);

  chassis.set_drive_pid(65, MAX_SPEED, true);
  chassis.wait_drive();

  l_grab.set_value(false);

  chassis.set_drive_pid(-10, MAX_SPEED, false);
  chassis.wait_drive();

  chassis.set_turn_pid(0, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-15, MAX_SPEED, false);
  chassis.wait_drive();

  f_grab.set_value(false);

  chassis.set_drive_pid(10, MAX_SPEED);
  chassis.wait_drive();

  chassis.set_swing_pid(ez::LEFT_SWING, 90, SWING_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(15, 70);
  chassis.wait_drive();
  
  l_grab.set_value(true);
  l.move_absolute(-2000, 200);

  chassis.set_drive_pid(-20, DRIVE_SPEED);
  chassis.wait_drive();
  
  l.move_absolute(200, 200);
  pros::delay(300);
  
  l_grab.set_value(false);

  chassis.set_drive_pid(-10, DRIVE_SPEED);
  chassis.wait_drive();
  
  
  f.move_absolute(-3800, 200);
  chassis.wait_drive();

  chassis.set_turn_pid(-90, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-10, 70);
  
}

void do_nothing() {}
