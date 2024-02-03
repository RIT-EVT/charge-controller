#include <EVT/utils/log.hpp>
#include <charge_controller/ChargeController.hpp>

namespace log = EVT::core::log;
ChargeController::ChargeController(BMSManager& bms, LCDDisplay& display, IO::CAN& can, DEV::Button& startButton, IO::GPIO& statusLED, ControllerUI& controllerUI, ControllerModel& controllerModel) : bms(bms), display(display), can(can), startButton(startButton), statusLED(statusLED), controllerUI(controllerUI), controllerModel(controllerModel) {}

void ChargeController::process() {
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

    bms.update();

    if (startButton.debounce(500)) {
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Start Pressed");
        if (isCharging()) {
            stopCharging();
        } else {
            startCharging();
        }
    }

    if (bms.numConnected() != oldCount) {
        log::LOGGER.log(log::Logger::LogLevel::DEBUG, "%d batteries connected", bms.numConnected());
        oldCount = bms.numConnected();
    }

    controllerUI.process();//calls the LCDDisplay display() method internally

    if (time::millis() - lastHeartBeat > HEARBEAT_INTERVAL) {
        switch (statusLED.readPin()) {
        case IO::GPIO::State::LOW:
            statusLED.writePin(IO::GPIO::State::HIGH);
            break;
        case IO::GPIO::State::HIGH:
            statusLED.writePin(IO::GPIO::State::LOW);
            break;
        }

        // Need to send heartbeat can message to the charger
        sendChargerMessage();
        lastHeartBeat = time::millis();
    }
}

uint8_t ChargeController::checkBMS() {
    uint8_t status = 0;

    /*
    TODO: was faulting on bms.getMaxCellVoltage(i)
     max voltage is 12000
     bms.getMaxCellVoltage(i) returned 25871
     */

    //    for (int i = 0; i < BMSManager::MAX_BMS_PACKS; i++) {
    //        if (bms.isConnected(i)) {
    //            log::LOGGER.log(log::Logger::LogLevel::INFO, "Battery %d", i);
    //            log::LOGGER.log(log::Logger::LogLevel::INFO, "Battery Voltage: %d", bms.getBatteryVoltage(i));
    //            log::LOGGER.log(log::Logger::LogLevel::INFO, "Battery MaxCellVoltage: %d", bms.getMaxCellVoltage(i));
    //            log::LOGGER.log(log::Logger::LogLevel::INFO, "Battery MinCellVoltage: %d", bms.getMinCellVoltage(i));
    //            log::LOGGER.log(log::Logger::LogLevel::INFO, "Battery BatteryMaxTemp: %d", bms.getBatteryMaxTemp(i));
    //            log::LOGGER.log(log::Logger::LogLevel::INFO, "Battery BatteryMinTemp: %d", bms.getBatteryMinTemp(i));
    //
    //            if (!CHECK_IN_RANGE(bms.getBatteryVoltage(i), MAX_PACK_VOLTAGE, MIN_PACK_VOLTAGE)) {
    //                //log::LOGGER.log(log::Logger::LogLevel::ERROR, "Pack Voltage %d: %d", i, bms.getBatteryVoltage(i));
    //
    //                status |= BAD_PACK_VOLTAGE;
    //            }
    //            if (!CHECK_IN_RANGE(bms.getMaxCellVoltage(i), MAX_CELL_VOLTAGE, MIN_CELL_VOLTAGE)) {
    //                //log::LOGGER.log(log::Logger::LogLevel::ERROR, "Max Cell Voltage %d: %d", i, bms.getMaxCellVoltage(i));
    //
    //                status |= BAD_MAX_CELL_VOLTAGE;
    //            }
    //            if (!CHECK_IN_RANGE(bms.getMinCellVoltage(i), MAX_CELL_VOLTAGE, MIN_CELL_VOLTAGE)) {
    //                status |= BAD_MIN_CELL_VOLTAGE;
    //            }
    //            if (!CHECK_IN_RANGE(bms.getBatteryMaxTemp(i), MAX_TEMPERATURE, MIN_TEMPERATURE)) {
    //                status |= BAD_MAX_TEMP;
    //            }
    //            if (!CHECK_IN_RANGE(bms.getBatteryMinTemp(i), MAX_TEMPERATURE, MIN_TEMPERATURE)) {
    //                status |= BAD_MIN_TEMP;
    //            }
    //
    //            if (bms.faultDetected(i)) {
    //                log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Fault State, Pack (%d)", i);
    //                status |= BAD_FAULT_STATE;
    //            }
    //        }
    //    }

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
        log::LOGGER.log(log::Logger::LogLevel::ERROR, "checkBMS: %d", checkBMS());
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
    /*
     * The following function may be confusing because it requires the use of
     * raw CAN messages. This is because of how the Elcon CAN specification
     * operates. The specification is documented in `datasheets/Elcon CAN Specification.pdf`
     */
    uint16_t voltage = 0;
    uint16_t current = 0;
    uint8_t stopCharging = 1;// Stop Charging

    if (state == ControllerStates::CHARGING) {
        // Multiply values by ten to get the right sized values
        voltage = controllerModel.getSavedVoltage() * 10;
        current = controllerModel.getSavedCurrent() * 10;
        stopCharging = 0;// Start Charging
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
        stopCharging,
        // Padding Bytes
        0x00,
        0x00,
        0x00};

    log::LOGGER.log(log::Logger::LogLevel::INFO, "Requesting charging at %d volts with %d current.", voltage / 10, current / 10);
    IO::CANMessage chargerMessage = IO::CANMessage(0x1806E5F4, 8, payload, true);
    IO::CAN::CANStatus status = can.transmit(chargerMessage);

    if (status == IO::CAN::CANStatus::ERROR) {
        log::LOGGER.log(log::Logger::LogLevel::INFO, "Error transmitting request %d", status);
    } else {
        log::LOGGER.log(log::Logger::LogLevel::INFO, "Transmitted request to charge controller");
    }
    log::LOGGER.log(log::Logger::LogLevel::INFO, "Current status: %d", status);
}

void ChargeController::setDisplayChargerValues(uint16_t voltage, uint16_t current) {
    display.setChargerVoltage(voltage);
    display.setChargerCurrent(current);
}
