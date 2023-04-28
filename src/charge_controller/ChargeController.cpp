#include <EVT/utils/log.hpp>
#include <charge_controller/ChargeController.hpp>

namespace log = EVT::core::log;
ChargeController::ChargeController(BMSManager& bms, LCDDisplay& display, IO::CAN& can) : bms(bms), display(display), can(can) {}

void ChargeController::loop() {
    switch (state) {
        case ControllerStates::NO_BATTERY:
            display.setChargeControllerStatus("No Battery");
            noBatteryState();
            break;
        case ControllerStates::CONNECTED:
            display.setChargeControllerStatus("Connected");
            connectedState();
            break;
        case ControllerStates::CHARGING:
            display.setChargeControllerStatus("Charging");
            chargingState();
            break;
        case ControllerStates::STANDBY:
            display.setChargeControllerStatus("Standby");
            standbyState();
            break;
        case ControllerStates::FAULT:
            display.setChargeControllerStatus("Fault");
            faultState();
            break;
    }

    if (bms.isConnected(0)) {
        display.setBatteryStatus(bms.getStatus(0), 0);
        display.setMinCellVoltage(bms.getMinCellVoltage(0), 0);
        display.setMinTemp(bms.getBatteryMinTemp(0), 0);
        display.setMaxTemp(bms.getBatteryMaxTemp(0), 0);
    } else {
        display.setBatteryStatus(BMSManager::BMSStatus::NOT_CONNECTED, 0);
    }

    if (bms.isConnected(1)) {
        display.setBatteryStatus(bms.getStatus(0), 1);
        display.setMinCellVoltage(bms.getMinCellVoltage(0), 1);
        display.setMinTemp(bms.getBatteryMinTemp(0), 1);
        display.setMaxTemp(bms.getBatteryMaxTemp(0), 1);
    } else {
        display.setBatteryStatus(BMSManager::BMSStatus::NOT_CONNECTED, 1);
    }

    display.display();

// Display the recieved current and voltage from the charger.
//    uint16_t voltage = ((uint16_t) payload[1] << 8) | payload[0];
//    uint16_t current = ((uint16_t) payload[3] << 8) | payload[2];
//
//    log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Payload: 0x%x, Current: %d. Voltage: %d", payload,  current, voltage);
//
//    display.setChargerCurrent(current);
//    display.setChargerVoltage(voltage);
}

uint8_t ChargeController::checkBMS() {
    bool status = 0;

    for (int i = 0; i < BMSManager::MAX_BMS_PACKS; i++) {
        if (bms.isConnected(i)) {
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
                log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Fault State, Pack (%d)", i);
                status |= BAD_FAULT_STATE;
            }
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
        // Display no battery connected message on LCD
//        relay.writePin(RELAY_OFF);
        changedState = false;
    }

    // When battery gets connected change state to CONNECTED for setup and initial
    // check
//    log::LOGGER.log(log::Logger::LogLevel::INFO, "Batt: %d", bms.numConnected());
    if (bms.numConnected()) {
        changedState = true;
        state = ControllerStates::CONNECTED;
    }
}

void ChargeController::connectedState() {
    if (changedState) {
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
//        relay.writePin(RELAY_ON);
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
//        relay.writePin(RELAY_OFF);
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
//        relay.writePin(RELAY_OFF);
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

void ChargeController::init() {
//    relay.writePin(RELAY_OFF);
}

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

void ChargeController::sendChargerMessage() {
    uint16_t voltage = 0;
    uint16_t current = 0;
    uint8_t shouldCharge = 1;

    if(state == ControllerStates::CHARGING) {
        // Multiply by ten to get the right sized values
        voltage = 48 * 10;
        current = 60 * 10;
        shouldCharge = 0;
    }

    uint8_t voltageMSB = voltage >> 8;
    uint8_t voltageLSB = voltage;

    uint8_t currentMSB = current >> 8;
    uint8_t currentLSB = current;

    uint8_t payload[8] = {
            voltageMSB,
            voltageLSB,
            currentMSB,
            currentLSB,
            shouldCharge,
            // Padding Bytes
            0x00,
            0x00,
            0x00
    };

    log::LOGGER.log(log::Logger::LogLevel::INFO, "Requesting charging at %d volts with %d current.", voltage / 10, current / 10);
    IO::CANMessage chargerMessage = IO::CANMessage(0x1806E5F4, 8, payload, true);
    IO::CAN::CANStatus status = can.transmit(chargerMessage);

    if (status == IO::CAN::CANStatus::ERROR) {
        log::LOGGER.log(log::Logger::LogLevel::INFO, "Error transmitting request %d", status);
    } else {
        log::LOGGER.log(log::Logger::LogLevel::INFO, "Transmitted request to charge controller");
    }
}

void ChargeController::setChargerValues(uint16_t voltage, uint16_t current) {
    display.setChargerVoltage(voltage);
    display.setChargerCurrent(current);
}

//void ChargeController::receiveChargerStatus() {
//    IO::CAN::CANStatus status = can.receive(&chargerStatusMessage);
//
//    if (status == IO::CAN::CANStatus::ERROR) {
//        log::LOGGER.log(log::Logger::LogLevel::INFO, "Error receiving status %d", status);
//    } else if (status == IO::CAN::CANStatus::TIMEOUT) {
//        log::LOGGER.log(log::Logger::LogLevel::INFO, "Charger status timed out, data not in message queue");
//    } else {
//        log::LOGGER.log(log::Logger::LogLevel::INFO, "Status successfully received!");
//    }
//}
