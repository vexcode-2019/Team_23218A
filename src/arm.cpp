#include "main.h"
#include <sstream>

const double max_height = -850;

using namespace okapi;

extern int left_port, right_port, lefttwo_port, righttwo_port,
    leftarm_port, rightarm_port, leftintake_port, rightintake_port;

extern std::shared_ptr<okapi::AsyncPosIntegratedController> lift;

void lift_stack(int cubes);

double major_positions[4] = { 0, 250, 500, 750 };
//double minor_positions[4] = {0.0,0.05,0.1,0.1};

std::shared_ptr<Arm> Arm::get() {
    static std::shared_ptr<Arm> instance(new Arm);
    return instance;
}

Arm::Arm() {
    peripherals->leftarm_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    peripherals->rightarm_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

    current_major_position = 0;
    current_minor_position = 0;
    user_pos_modifier = 0;
    sensitivity = 0.0001;
}

void Arm::user_control() {
    int power = peripherals->master_controller.get_analog(ANALOG_LEFT_Y);

    //bool arm_up = peripherals->master_controller.get_digital_new_press(DIGITAL_UP);
    //bool arm_down = peripherals->master_controller.get_digital_new_press(DIGITAL_DOWN);
    //int block_up = peripherals->master_controller.get_digital_new_press(DIGITAL_R1);
    //int block_down = peripherals->master_controller.get_digital_new_press(DIGITAL_R2);

    bool tare = peripherals->master_controller.get_digital_new_press(DIGITAL_B);
    bool stack = peripherals->master_controller.get_digital_new_press(DIGITAL_DOWN);

    if (tare) {
        peripherals->leftarm_mtr.tare_position();
        peripherals->rightarm_mtr.tare_position();
    }
    if (stack) {
        lift_stack(4);
    }
    /*
    if (arm_up && (current_major_position <= 3)) {
        current_major_position++;
    } else if (arm_down && (current_major_position > 0)) {
        current_major_position--;
    }
    */
    /*
      if (block_up && (current_minor_position <= 3)){
        current_minor_position++;
      }
      else if (block_down&&(current_minor_position > 0)){
        current_minor_position--;
      }
      */
    //user_pos_modifier += (double)power * sensitivity;
    double final_height = major_positions[current_major_position] /* +
                    minor_positions[current_minor_position]*/
        ;

    height_per = abs(peripherals->leftarm_mtr.get_position()) / abs(max_height);

    // Quick and dirty place to put this
    std::string arm_pos = std::to_string(peripherals->leftarm_mtr.get_position());
    std::string temp = std::to_string(peripherals->leftarm_mtr.get_temperature());
    temp.append(" celcius");
    pros::lcd::set_text(1, arm_pos);

    pros::lcd::set_text(2, temp);
    //double power_mult = (peripherals->leftarm_mtr.get_actual_velocity() > 1 &&
    //height_per < 0.2) ? 0.01:1;

    //double power_mult = 1;
    power = power /* * power_mult*/;
    if (power > 5 || power < 5) {
        if (!lift->isDisabled())
            lift->flipDisable(true);

        this->set(power);
        lift->setTarget(peripherals->leftarm_mtr.get_position());
    } else {
        if (lift->isDisabled())
            lift->flipDisable(false);
    }
}

void Arm::set(int power) {
    //if (abs(power) < 10) power = 5;
    peripherals->leftarm_mtr.move(-power);
    peripherals->rightarm_mtr.move(-power);
}
void Arm::set_pos(double position) {
    /*
    if (position < 0.1) {
      peripherals->leftarm_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
      peripherals->rightarm_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    }
    else{
      peripherals->leftarm_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
      peripherals->rightarm_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
      }
    */
    peripherals->leftarm_mtr.move_absolute(-position, 63);
    peripherals->rightarm_mtr.move_absolute(-position, 63);
}
