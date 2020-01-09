#include "main.h"

// The autonomous framework that is a godsend
using namespace okapi;

/*
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

extern int8_t left_port, right_port, lefttwo_port, righttwo_port,
    leftarm_port, rightarm_port, intake_port, strafe_port;

const auto WHEEL_DIAMETER = 4.3_in;
const auto CHASSIS_WIDTH = 370_mm;
const auto INTAKE_FROM_CENTER = 12.5_in;

std::shared_ptr<okapi::ChassisController> ccont;
std::shared_ptr<Motor> intake;
std::shared_ptr<ChassisControllerHDrive> cc;
pros::Mutex cc_mutex;
// This mutex is not actually used for the controller, rather the shared_ptr
// We  don't want two threads trying to set/reset this pointer!

//extern std::shared_ptr<okapi::AsyncPositionController<double, double>> lift;

extern pros::ADIAnalogIn arm_pot;

void open_claw() {
    intake->moveVoltage(-12000);
    while (intake->getActualVelocity() > 5)
        pros::delay(10);
    intake->moveVoltage(0);
}

void close_claw() {
    intake->moveVoltage(12000);
    while (intake->getActualVelocity() > 5)
        pros::delay(10);
}

void pot_lookup() {
    cc->stop_task();
    Arm::get()->flipDisable(true);
    fprintf(stderr, "starting");
    std::ofstream save_file("/usd/yeet.csv", std::ofstream::out | std::ofstream::trunc);
    save_file.clear();
    peripherals->leftarm_mtr.move_velocity(30);
    peripherals->rightarm_mtr.move_velocity(30);
    for (int i = 0; i < 500; i++) {
        save_file << peripherals->leftarm_mtr.get_position() << ",";
        save_file << arm_pot.get_value() << "\n";
        pros::delay(10);
    }
    save_file.close();
    fprintf(stderr, "stopped");
};
void vision_test() {
    fprintf(stderr, "waiting for yeet");

    /*
    cc->driveToPoint({ 2_ft, 0_ft });
    cc->driveToPoint({ 0_ft, 0_ft });
    cc->lookToPoint({ 1_ft, 0_ft });
    */
    cc->generatePath({ { 0_ft, 0_ft, 0_deg }, { 2_ft, 0_ft, 0_deg } }, "A");
    cc->runPath("A", false, false);
    //return;
    /*
    cc->stop_task();

    auto profileController = AsyncMotionProfileControllerBuilder()
                             .withLimits({0.5, 0.5, 0.5})
                             .withOutput(ccont)
                             .buildMotionProfileController();
    profileController->generatePath({{0_ft, 0_ft, 0_deg}, {2_ft, -2_ft, -90_deg}}, "A");
    profileController->setTarget("A");
    profileController->waitUntilSettled();
    */

    //while (true)
    //    pros::delay(100);
    //return;
    //vision_signature_s_t sig = pros::Vision::signature_from_utility ( 1, 607, 2287, 1446, 6913, 10321, 8618, 3.000, 0 );
    //vision::signature SIG_1 (1, 607, 2287, 1446, 6913, 10321, 8618, 3.000, 0);
}

// Starts pointing towards small goal zone
void near_small() {
    ccont->moveDistance(17 * inch);
    ccont->moveDistance(-18_in);
}

void do_nothing() {
    while (true)
        pros::delay(100);
};

void move_15() {
    cc->driveStraight(18_in);
    cc->driveStraight(-18_in);
}

void arm_test() {
    //cc->setHeading(90_deg);
    cc->strafe(6_in);
    return;
    std::shared_ptr<Arm> arm = Arm::get();
    arm->set_height(10_in);
    arm->waitUntilSettled();
}
/*
void four_stack() {
    int side = ConfigManager::get()->selected_team;
    std::shared_ptr<Arm> arm = Arm::get();
    okapi::QAngle inward;
    if (side < 0)
        inward = 180_deg;
    else
        inward = 0_deg;
    arm->flipDisable(true);
    cc->driveStraight(5_in);
    arm->flipDisable(false);
    arm->set_height(2.5_in);
    arm->waitUntilSettled();
    cc->driveToPoint({ 105.7_in, 49.9_in - INTAKE_FROM_CENTER });
    cc->setHeading(inward);
    cc->strafe(97.1_in - 105.7_in);
    intake->moveVoltage(12000);
    pros::delay(500);
    arm->set_height(15_in);
    cc->driveStraight(-2_ft);
    cc->driveToPoint({ 11.5_ft - INTAKE_FROM_CENTER, 1_ft });
    cc->lookToPoint({ 13_ft, 1_ft });
    //cc->strafe(side * 1_ft);
    arm->flipDisable(false);
    arm->set_height(0_in);
    arm->waitUntilSettled();
    arm->flipDisable(true);
    arm->set(-200);
    pros::delay(500);
    intake->moveVoltage(-12000);
}
*/
void simpler_four_stack() {
    int side = ConfigManager::get()->selected_team;
    std::shared_ptr<Arm> arm = Arm::get();

    // Get to cube
    arm->flipDisable(true);
    auto cubeydelta = (48.2_in - INTAKE_FROM_CENTER) - cc->odom->getState(okapi::StateMode::CARTESIAN).y;
    cc->driveStraight(cubeydelta);
    arm->flipDisable(false);
    arm->set_height(2.3_in);
    cc->strafe((97.1_in - cc->odom->getState(okapi::StateMode::CARTESIAN).x) + (3_in * side * -1));
    arm->waitUntilSettled();

    cc->setHeading(0_deg);
    cc->driveStraight(1.2_in);
    intake->moveVoltage(12000);
    pros::delay(500);
    arm->set_height(7_in);
    cc->driveStraight(-1.5_ft);
    auto large_side = (side > 0) ? 11.5_ft - INTAKE_FROM_CENTER : 58.6_in + INTAKE_FROM_CENTER;
    cc->driveToPoint({ large_side, 9_in });
    cc->lookToPoint({ large_side + (1_ft * side), cc->odom->getState(okapi::StateMode::CARTESIAN).y });

    //cc->driveStraight(1_in); // NOTE: this shouldn't be necessary but is
    //cc->strafe(0.2_ft- cc->odom->getState(okapi::StateMode::CARTESIAN).x );
    arm->flipDisable(false);
    arm->set_height(0_in);
    arm->waitUntilSettled();
    arm->flipDisable(true);
    arm->set(-200);
    pros::delay(500);
    intake->moveVoltage(-12000);
    pros::delay(500);
    arm->set(200);
    intake->moveVoltage(0);
    cc->driveStraight(-2_in);
    arm->set(0);
}

void new_four_stack() {
    int side = ConfigManager::get()->selected_team;
    std::shared_ptr<Arm> arm = Arm::get();

    // Get to cube
    arm->flipDisable(true);

    auto nearcubeydelta = 26.4_in - cc->odom->getState(okapi::StateMode::CARTESIAN).y;
    cc->driveStraight(nearcubeydelta);
    arm->flipDisable(false);
    arm->set_height(2.3_in);
    cc->strafe((97.1_in - cc->odom->getState(okapi::StateMode::CARTESIAN).x) + (3_in * side * -1));
    cc->setHeading(0_deg);
    auto cubeydelta = (49.9_in - INTAKE_FROM_CENTER) - cc->odom->getState(okapi::StateMode::CARTESIAN).y;
    cc->driveStraight(cubeydelta);

    arm->waitUntilSettled();
    intake->moveVoltage(12000);
    pros::delay(500);
    arm->set_height(3.5_in);
    cc->setHeading((side > 0) ? 90_deg : -90_deg);

    auto large_side = (side > 0) ? 11.5_ft - INTAKE_FROM_CENTER : 58.6_in + INTAKE_FROM_CENTER;
    cc->strafe((8_in - cc->odom->getState(okapi::StateMode::CARTESIAN).y));
    cc->driveToPoint({ large_side, 8_in });
    cc->lookToPoint({ large_side + (1_ft * side), cc->odom->getState(okapi::StateMode::CARTESIAN).y });

    //cc->driveStraight(1_in); // NOTE: this shouldn't be necessary but is
    //cc->strafe(0.2_ft- cc->odom->getState(okapi::StateMode::CARTESIAN).x );
    arm->flipDisable(false);
    arm->set_height(0_in);
    arm->waitUntilSettled();
    arm->flipDisable(true);
    arm->set(-200);
    pros::delay(500);
    intake->moveVoltage(-12000);
    pros::delay(500);
    arm->set(200);
    intake->moveVoltage(0);
    cc->driveStraight(-2_in);
    arm->set(0);
}

void seven_stack() {
    int side = ConfigManager::get()->selected_team;
    std::shared_ptr<Arm> arm = Arm::get();
    okapi::QAngle inward;
    if (side < 0)
        inward = 180_deg;
    else
        inward = 0_deg;

    auto cubeydelta = (49.9_in - INTAKE_FROM_CENTER) - cc->odom->getState(okapi::StateMode::CARTESIAN).y;
    cc->driveStraight(cubeydelta);
    arm->set_height(2.5_in);
    cc->strafe((70.3_in + 2.5_in) - cc->odom->getState(okapi::StateMode::CARTESIAN).x);
    arm->waitUntilSettled();
    intake->moveVoltage(12000);

    cc->driveToPoint({ 11.5_ft - INTAKE_FROM_CENTER, 1_ft });
    cc->lookToPoint({ 13_ft, 1_ft });
    arm->flipDisable(false);
    arm->set_height(0_in);
    arm->waitUntilSettled();
    arm->flipDisable(true);
    arm->set(-200);
    pros::delay(500);
    intake->moveVoltage(-12000);
    pros::delay(500);
    arm->set(0);
    intake->moveVoltage(0);
    pros::delay(500);
    cc->driveStraightAsync(-5_in);
    arm->set_height(0.1_in);
    cc->waitUntilSettled();

    cc->driveToPoint({ 105.7_in, 49.9_in - INTAKE_FROM_CENTER });
    arm->set_height(2.5_in);
    cc->setHeading(inward);
    cc->strafe(97.1_in - cc->odom->getState(okapi::StateMode::CARTESIAN).x);
    arm->waitUntilSettled();

    intake->moveVoltage(12000);
    pros::delay(500);
    arm->set_height(7_in);
    cc->driveToPoint({ 1_ft, 11.5_ft - INTAKE_FROM_CENTER });
    arm->set_height(22_in);
    arm->waitUntilSettled();
    cc->lookToPoint({ 13_ft, 1_ft });

    arm->set_height(21_in);
    pros::delay(100);
    intake->moveVoltage(-12000);
    arm->waitUntilSettled();
    cc->driveStraight(-1_in);
}

void four_floor() {
    int side = ConfigManager::get()->selected_team;
    std::shared_ptr<Arm> arm = Arm::get();
    okapi::QAngle inward;
    if (side < 0)
        inward = 180_deg;
    else
        inward = 0_deg;

    intake->moveVoltage(12000);
    arm->set_height(5.5_in);
    cc->driveToPoint({ 26.4_in, 33.4_in - INTAKE_FROM_CENTER });
    cc->setHeading(inward);
    for (int i = 0; i < 3; i++) {
        intake->moveVoltage(12000);
        pros::delay(500);
        arm->set_height(5.5_in);
        arm->waitUntilSettled();
        cc->driveStraight(5.5_ft);
        intake->moveVoltage(-12000);
        pros::delay(500);
        arm->set_height(0_in);
    }
    arm->set_height(2.3_in);
    intake->moveVoltage(12000);
    pros::delay(500);
    arm->set_height(3.5_in);

    cc->driveToPoint({ 6_in + (INTAKE_FROM_CENTER * sin(cc->odom->getState(okapi::StateMode::CARTESIAN).theta.convert(okapi::radian))),
                    6_in + (INTAKE_FROM_CENTER * cos(cc->odom->getState(okapi::StateMode::CARTESIAN).theta.convert(okapi::radian))) });
    arm->set_height(0_in);
    arm->waitUntilSettled();

    intake->moveVoltage(-12000);
    pros::delay(500);
    arm->set_height(5_in);
    intake->moveVoltage(0);
    cc->driveStraight(-2_in);
}

void bob_auton(){
    int side = ConfigManager::get()->selected_team;
    std::shared_ptr<Arm> arm = Arm::get();

    auto small_side = 0.5_ft + INTAKE_FROM_CENTER;

    //cube 1
    close_claw();
    arm->set_height(6.5_in);

    //cube 2
    cc->driveStraight((33.4_in - INTAKE_FROM_CENTER) - cc->odom->getState(okapi::StateMode::CARTESIAN).y);
    arm->set_height(5.5_in);
    open_claw();
    arm->set_height(0_in);
    arm->waitUntilSettled();
    close_claw();
    arm->set_height(6.5_in);
    arm->waitUntilSettled();

    //cube 3
    cc->driveStraight((38.9_in - INTAKE_FROM_CENTER) - cc->odom->getState(okapi::StateMode::CARTESIAN).y);
    arm->set_height(5.5_in);
    open_claw();
    arm->set_height(0_in);
    arm->waitUntilSettled();
    close_claw();
    arm->set_height(6.5_in);
    arm->waitUntilSettled();

    //cube 4
    cc->driveStraight((44.4_in - INTAKE_FROM_CENTER) - cc->odom->getState(okapi::StateMode::CARTESIAN).y);
    arm->set_height(5.5_in);
    open_claw();
    arm->set_height(2.3_in);
    arm->waitUntilSettled();
    close_claw();
    arm->set_height(6.5_in);
    arm->waitUntilSettled();

    cc->driveStraight(-1.5_ft);
    cc->driveToPoint({ small_side, 4.5_in });
    cc->lookToPoint({ small_side + (1_ft * side), cc->odom->getState(okapi::StateMode::CARTESIAN).y });

    arm->set_height(2_in);
    arm->waitUntilSettled();
    open_claw();
    arm->set_height(5_in);
    cc->driveStraight(-4_in);
}

/* ----------------------------------------------------------------
   AUTON HOUSE KEEPING FUNCTIONS
   ---------------------------------------------------------------- */

void create_cc() {
    PIDTuning straightTuning = PIDTuning(0.001890, 0.0, 0.000019);
    PIDTuning angleTuning = PIDTuning(0.000764, 0, 0.000007);
    PIDTuning turnTuning = PIDTuning(0.001500, 0, 0.000053);
    PIDTuning strafeTuning = PIDTuning(0.002, 0, 0.00003);
    PIDTuning hypotTuning = PIDTuning(0, 0, 0);
    okapi::MotorGroup leftSide(
        { left_port, lefttwo_port });
    okapi::MotorGroup rightSide(
        { right_port, righttwo_port });
    okapi::Motor strafeMotor(strafe_port);

    // std::unique_ptr<ChassisControllerHDrive>
    cc = std::make_unique<ChassisControllerHDrive>(
        straightTuning, angleTuning, turnTuning, strafeTuning, hypotTuning, // Tunings
        leftSide, rightSide, strafeMotor, // left mtr, right mtr, strafe mtr
        okapi::AbstractMotor::gearset::green, // swerve steer gearset
        okapi::AbstractMotor::gearset::green, // strafe gearset
        okapi::ChassisScales(
            { { okapi::inch * 4.125, 15.1 * okapi::inch, // wheel diam, wheelbase diam
                  0 * okapi::millimeter, okapi::inch * 4.125 }, // middle wheel distance, middle wheel diam
                okapi::imev5GreenTPR }));
}

void init_autonomous() {
    ccont = ChassisControllerBuilder()
                .withMotors({ left_port, lefttwo_port },
                    { right_port, righttwo_port })
                .withDimensions(AbstractMotor::gearset::green, { { WHEEL_DIAMETER, CHASSIS_WIDTH }, imev5GreenTPR })
                .build();

    intake = std::make_unique<okapi::Motor>(intake_port);

    Arm::get()->flipDisable(true);

    if (!cc)
        create_cc();

    auto configManager = ConfigManager::get();
    configManager->register_auton("near small", near_small);
    configManager->register_auton("do nothing", do_nothing);
    configManager->register_auton("four stack", simpler_four_stack,
        okapi::OdomState{ 97.1_in, 26.4_in - INTAKE_FROM_CENTER, 0_deg });
    configManager->register_auton("strafey new four stack", new_four_stack,
        okapi::OdomState{ 97.1_in, 26.4_in - INTAKE_FROM_CENTER, 0_deg });

    configManager->register_auton("bob auton", bob_auton,
        okapi::OdomState{ 26.4_in, 33.4_in - INTAKE_FROM_CENTER, 0_deg });
    configManager->register_auton("four floor", four_floor,
        okapi::OdomState{ 26.4_in, 33.4_in - INTAKE_FROM_CENTER, 0_deg });

    configManager->register_auton("Move 15", move_15);
    configManager->register_auton("arm test", arm_test);
    //configManager->register_auton("potentiomenter test", pot_lookup);
    //configManager->register_auton("vision test", vision_test);
}

void auton_cleanup() {
    Arm::get()->flipDisable(true);
    cc_mutex.take(TIMEOUT_MAX);
    if (cc) {
        cc->stop_task();
        cc->reset();
    }
    cc_mutex.give();
}
void auton_cleanup_task(void* param) {
    pros::c::task_notify_take(true, TIMEOUT_MAX);
    auton_cleanup();
}

void autonomous() {
    pros::delay(150); // Counter ADI garbage
    auto our_cleanup_task = pros::Task(auton_cleanup_task, NULL, TASK_PRIORITY_DEFAULT,
        TASK_STACK_DEPTH_DEFAULT, "Auton cleanup");

    pros::c::task_notify_when_deleting(CURRENT_TASK, our_cleanup_task, 0,
        pros::E_NOTIFY_ACTION_NONE);

    if (!cc)
        create_cc();

    // Run your standard auton
    std::shared_ptr<ConfigManager> configManager = ConfigManager::get();
    std::shared_ptr<Arm> arm = Arm::get();
    arm->set_height(0_in);
    if (configManager->auton_routines.size() > configManager->selected_auton) {
        auton_func routine = configManager->get_auton_func(configManager->selected_auton);
        cc->start_task();
        arm->flipDisable(false);
        routine(); // nullptr could happen, lets hope it doesn't :o
        arm->flipDisable(true);
        cc->stop_task();
        //auton_task.remove();
    } else {
        printf("Selected auton is greater than amount of autons");
    }
    auton_cleanup();
}

/*
blue small side - be lined up with the 4 long tihng of cubes
  grip the pre-load
  forward(distance to cubes...)
  armUp(height of one cube)
  forward(length of cube)
  armUn-Grab
  repeat 4 times:
    armDown(height of one cubes)
    armGrab
    armUp(height of one cube)
    fowrard(length of one cube)
    armUn-Grab
  backwards(one tile)w
  turnLeft(135Deg)
  forward(34 inches)
  armUn-Grab
  */
