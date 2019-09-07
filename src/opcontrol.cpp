

#include "main.h"

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
extern Chassis chassis;
extern Arm arm;
extern Claw claw;

void opcontrol() {

	while (true) {
		macros_update(peripherals.master_controller);

    arm.user_control();
    chassis.user_control();
		claw.user_control();
		
		pros::delay(20);
	}
}
