#include <charge_controller/ChargeController.hpp>

ChargeController::ChargeController(BMSManager bms, LCDDisplay display,
                                   IO::GPIO &relay)
        : bms(bms), lcdDisplay(display), relay(relay) {}

void ChargeController::loop() {
    switch (state) {
        case ControllerStates::NO_BATTERY:
            lcdDisplay.setChargeControllerStatus("No Battery");
            noBatteryState();
            break;
        case ControllerStates::CONNECTED:
            lcdDisplay.setChargeControllerStatus("Connected");
            connectedState();
            break;
        case ControllerStates::CHARGING:
            lcdDisplay.setChargeControllerStatus("Charging");
            chargingState();
            break;
        case ControllerStates::STANDBY:
            lcdDisplay.setChargeControllerStatus("Standby");
            standbyState();
            break;
        case ControllerStates::FAULT:
            lcdDisplay.setChargeControllerStatus("Fault");
            faultState();
            break;
    }
    // Display LCD values
    lcdDisplay.display();
}

/**
 * checks the status of the bms and that all measurements are in spec
 * @return true when everything ok, false if there is a problem
 */
bool ChargeController::checkBMS() {
    bool status = true;
    int temperature = bms.getBattTemperature();
    int voltage = bms.getBattVoltage();

    if (temperature > MAX_TEMPERATURE) {
        status = false;
    }
    if (voltage > MAX_VOLTAGE) {
        status = false;
    }
    if (bms.faultDetected()) {
        status = false;
    }
    return status;
}

/**
 * state when the battery is disconnected from the charge controller
 * shut off the relay and wait for a connection.
 */
void ChargeController::noBatteryState() {
    if (changedState) {
        // Display no battery connected message on LCD
        relay.writePin(RELAY_OFF);
        changedState = false;
    }

    // When battery gets connected change state to CONNECTED for setup and initial
    // check
    if (bms.isConnected()) {
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
        // display connected LCD message
        // further init battery communication?
        changedState = false;
    }
    if (checkBMS()) {
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
        relay.writePin(RELAY_ON);
        changedState = false;
    }
    if (!bms.isConnected()) {
        changedState = true;
        state = ControllerStates::NO_BATTERY;
    } else if (!checkBMS()) {
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
        relay.writePin(RELAY_OFF);
        changedState = false;
    }

    if (!bms.isConnected()) {
        changedState = true;
        state = ControllerStates::NO_BATTERY;
    } else if (!checkBMS()) {
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
        relay.writePin(RELAY_OFF);
        changedState = false;
    }

    if (!bms.isConnected()) {
        changedState = true;
        state = ControllerStates::NO_BATTERY;
    } else if (checkBMS()) {
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
    if (state == ControllerStates::STANDBY) {
        state = ControllerStates::CHARGING;
        changedState = true;
    }
}

/**
 * stop charging when the stop button is pressed
 */
void ChargeController::stopCharging() {
    if (state == ControllerStates::CHARGING) {
        state = ControllerStates::STANDBY;
        changedState = true;
    }
}
