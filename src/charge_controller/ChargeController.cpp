#include <charge_controller/ChargeController.hpp>
#include <charge_controller/Logger.h>

ChargeController::ChargeController(BMSManager &bms, LCDDisplay &display,
                                   IO::GPIO& relay)
    : bms(bms), display(display), relay(relay) {}

void ChargeController::loop() {
    switch (state) {
    case ControllerStates::NO_BATTERY:
        noBatteryState();
        break;
    case ControllerStates::CONNECTED:
        connectedState();
        break;
    case ControllerStates::CHARGING:
        chargingState();
        break;
    case ControllerStates::STANDBY:
        standbyState();
        break;
    case ControllerStates::FAULT:
        faultState();
        break;
    }
}

/**
 * checks the status of the bms and that all measurements are in spec
 * @return true when everything ok, false if there is a problem
 */
uint8_t ChargeController::checkBMS() {
    bool status = 0;
//    int temperature = bms.getBattTemperature();
//    int voltage = bms.getBattVoltage();
    for (int i = 0; i < bms.MAX_BMS_PACKS; i++) {
        if (!CHECK_IN_RANGE(bms.getBatteryVoltage(i), MAX_PACK_VOLTAGE, MIN_PACK_VOLTAGE)){
            status |= BAD_PACK_VOLTAGE;
            //LOG.log(Logger::DEBUG, "Pack %d, Bad Voltage: %dV", i, bms.getBatteryVoltage(i));
        }
        if(!CHECK_IN_RANGE(bms.getMaxCellVoltage(i), MAX_CELL_VOLTAGE, MIN_CELL_VOLTAGE)){
            status |= BAD_MAX_CELL_VOLTAGE;
            //LOG.log(Logger::DEBUG, "Pack %d, Bad Max Cell(%d) Voltage: %dV", i, bms.getMaxCellVoltageID(i), bms.getMaxCellVoltage(i));
        }
        if(!CHECK_IN_RANGE(bms.getMinCellVoltage(i), MAX_CELL_VOLTAGE, MIN_CELL_VOLTAGE)){
            status |= BAD_MIN_CELL_VOLTAGE;
            //LOG.log(Logger::DEBUG, "Pack %d, Bad Min Cell(%d) Voltage: %dV", i, bms.getMinCellVoltageID(i), bms.getMinCellVoltage(i));
        }
        if(!CHECK_IN_RANGE(bms.getBatteryMaxTemp(i), MAX_TEMPERATURE, MIN_TEMPERATURE)){
            status |= BAD_MAX_TEMP;
            //LOG.log(Logger::DEBUG, "Pack %d, Bad Max Temp: %d", i, bms.getBatteryMaxTemp(i));
        }
        if(!CHECK_IN_RANGE(bms.getBatteryMinTemp(i), MAX_TEMPERATURE, MIN_TEMPERATURE)){
            status |= BAD_MIN_TEMP;
            //LOG.log(Logger::DEBUG, "Pack %d, Bad Min Temp: %d", i, bms.getBatteryMinTemp(i));
        }
        if(bms.faultDetected(i)){
            status |= BAD_FAULT_STATE;
        }
    }
    return status;
}

/**
 * state when the battery is disconnected from the charge controller
 * shut off the relay and wait for a connection.
 */
void ChargeController::noBatteryState() {
    if (changedState) {
        LOG.log(Logger::DEBUG, "Changed state->No Battery");
        // Display no battery connected message on LCD
        relay.writePin(RELAY_OFF);
        changedState = false;
    }

    // When battery gets connected change state to CONNECTED for setup and initial
    // check
    if (bms.numConnected()) {
        changedState = true;
        state = ControllerStates::CONNECTED;
    }
}

/**
 * battery connected state
 * initialize connection with the bms and do initial safety check
 * if bms is ready to charge, wait in standby for user to start charge
 */
void ChargeController::connectedState() {
    if (changedState) {
        LOG.log(Logger::DEBUG, "Changed state->Connected State");
        LOG.log(Logger::DEBUG, "%d pack(s) connected", bms.numConnected());
        // display connected LCD message
        // further init battery communication?
        changedState = false;
    }
    if (checkBMS() == 0) {
        changedState = true;
        state = ControllerStates::STANDBY;
    } else {
        changedState = true;
        state = ControllerStates::FAULT;
    }
}

/**
 * battery charging state
 * monitor the battery while it charges. If a fault occurs transition to fault
 * state
 */
void ChargeController::chargingState() {
    if (changedState) {
        LOG.log(Logger::DEBUG, "Changed state->Charging State");
        relay.writePin(RELAY_ON);
        changedState = false;
    }
    if (bms.numConnected() == 0) {
        changedState = true;
        state = ControllerStates::NO_BATTERY;
    } else if (checkBMS() != 0) {
        changedState = true;
        state = ControllerStates::FAULT;
    }
}

/**
 * standby state
 * wait for user input to start charging as long as the BMS is ok
 */
void ChargeController::standbyState() {
    if (changedState) {
        LOG.log(Logger::DEBUG, "Changed state->Standby State, waiting for button");
        relay.writePin(RELAY_OFF);
        changedState = false;
    }

    if (bms.numConnected() == 0) {
        changedState = true;
        state = ControllerStates::NO_BATTERY;
    } else if (checkBMS() != 0) {
        changedState = true;
        state = ControllerStates::FAULT;
    }
}

/**
 * Fault occurred state
 * A fault has occurred, stop charging. When fault is cleared, wait in standby
 */
void ChargeController::faultState() {
    if (changedState) {
        LOG.log(Logger::DEBUG, "Changed state->Fault State");
        relay.writePin(RELAY_OFF);
        changedState = false;
    }

    if (bms.numConnected() == 0) {
        changedState = true;
        state = ControllerStates::NO_BATTERY;
    } else if (checkBMS() == 0) {
        changedState = true;
        state = ControllerStates::STANDBY;
    }
}

void ChargeController::init() { relay.writePin(RELAY_OFF); }

/**
 * when the start button is pressed, start charging if the bms is ready to
 * charge
 */
void ChargeController::startCharging() {
    switch (state) {
        case ControllerStates::STANDBY:
            LOG.log(Logger::DEBUG, "Start button pressed, starting charge");
            break;
        case ControllerStates::CHARGING:
            break;
        default:
            LOG.log(Logger::DEBUG, "Start button pressed, Bad State");
            break;
    }

    if (state == ControllerStates::STANDBY) {
        state = ControllerStates::CHARGING;
        changedState = true;
    }
}

/**
 * stop charging when the stop button is pressed
 */
void ChargeController::stopCharging() {
    LOG.log(Logger::DEBUG, "Stop button pressed, stopping charge");
    if (state == ControllerStates::CHARGING) {
        state = ControllerStates::STANDBY;
        changedState = true;
    }
}
