#include "main.h"
#include <iomanip>
#include <sstream>
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

void set_temperature(int i) {
    std::shared_ptr<GUI> gui = GUI::get();
    std::string lift_imbalance_str = "Imbalance to the left: ";

    int chassis_temp = (peripherals->left_mtr.get_temperature() + peripherals->right_mtr.get_temperature() + peripherals->lefttwo_mtr.get_temperature() + peripherals->righttwo_mtr.get_temperature()) / 4;
    int arm_temp = (peripherals->leftarm_mtr.get_temperature() + peripherals->rightarm_mtr.get_temperature()) / 2;
    int claw_temp = peripherals->intake_mtr.get_temperature();

    float leftarm_pwr = peripherals->leftarm_mtr.get_power();
    float rightarm_pwr = peripherals->rightarm_mtr.get_power();
    float smallest_arm_pwr = std::min(leftarm_pwr, rightarm_pwr);
    float normalised_imbalance = (leftarm_pwr / smallest_arm_pwr) - (rightarm_pwr / smallest_arm_pwr);

    std::stringstream stream;
    stream << lift_imbalance_str << std::fixed << std::setprecision(6) << normalised_imbalance;
    lift_imbalance_str = stream.str();

    if (i == 3)
        peripherals->master_controller.print(0, 0, "Arm,Chassis,Claw");
    if (i == 2)
        peripherals->master_controller.print(1, 0, "%d, %d, %d", arm_temp, chassis_temp, claw_temp);

    gui->set_line(0, lift_imbalance_str);

    lv_gauge_set_value(gui->chassis_temp_guage, 0, chassis_temp);
    lv_gauge_set_value(gui->arm_temp_guage, 0, arm_temp);
    lv_gauge_set_value(gui->claw_temp_guage, 0, claw_temp);
}

void opcontrol() {
    std::shared_ptr<Chassis> chassis = Chassis::get();
    std::shared_ptr<Arm> arm = Arm::get();
    std::shared_ptr<Claw> claw = Claw::get();

    peripherals->master_controller.print(-1, 0, "");

    int it = 12;
    while (true) {
        //lv_ta_add_text(GUI::get()->console_box,std::to_string(pros::millis()).c_str());
        //cc->step();
        //macros_update(peripherals->master_controller);

        arm->user_control();
        chassis->user_control();
        claw->user_control();

        it--;
        if (it == 0)
            it = 50;
        if (it <= 9 && it % 3 == 0) // Trying to delay over 50ms but not using tasks ...
            set_temperature(it / 3);

        pros::delay(20);
    }
}
