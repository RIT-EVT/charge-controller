#include <EVT/utils/log.hpp>
#include <charge_controller/ChargeController.hpp>

namespace log = EVT::core::log;
ChargeController::ChargeController(BMSManager bms, LCDDisplay& display,
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

uint8_t ChargeController::checkBMS() {
    bool status = 0;

    for (int i = 0; i < bms.MAX_BMS_PACKS; i++) {
        if (!CHECK_IN_RANGE(bms.getBatteryVoltage(i), MAX_PACK_VOLTAGE, MIN_PACK_VOLTAGE)) {
            status |= BAD_PACK_VOLTAGE;
        }
        if (!CHECK_IN_RANGE(bms.getMaxCellVoltage(i), MAX_CELL_VOLTAGE, MIN_CELL_VOLTAGE)) {
            status |= BAD_MAX_CELL_VOLTAGE;
        }
        if (!CHECK_IN_RANGE(bms.getMinCellVoltage(i), MAX_CELL_VOLTAGE, MIN_CELL_VOLTAGE)) {
            status |= BAD_MIN_CELL_VOLTAGE;
        }
        if (!CHECK_IN_RANGE(bms.getBatteryMaxTemp(i), MAX_TEMPERATURE, MIN_TEMPERATURE)) {
            status |= BAD_MAX_TEMP;
        }
        if (!CHECK_IN_RANGE(bms.getBatteryMinTemp(i), MAX_TEMPERATURE, MIN_TEMPERATURE)) {
            status |= BAD_MIN_TEMP;
        }
        if (bms.faultDetected(i)) {
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
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Changed state->No Battery");
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


void ChargeController::connectedState() {
    if (changedState) {
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Changed state->Connected State");
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "%d pack(s) connected", bms.numConnected());
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

void ChargeController::chargingState() {
    if (changedState) {
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Changed state->Charging State");
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

void ChargeController::standbyState() {
    if (changedState) {
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Changed state->Standby State, waiting for button");
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

void ChargeController::faultState() {
    if (changedState) {
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Changed state->Fault State");
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


void ChargeController::startCharging() {
    switch (state) {
    case ControllerStates::STANDBY:
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Start button pressed, starting charge");
        break;
    case ControllerStates::CHARGING:
        break;
    default:
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Start button pressed, Bad State");
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
    log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Stop button pressed, stopping charge");
    if (state == ControllerStates::CHARGING) {
        state = ControllerStates::STANDBY;
        changedState = true;
    }
}
