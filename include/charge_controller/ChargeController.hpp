#ifndef CHARGE_CONTROLLER_H
#define CHARGE_CONTROLLER_H

#include <EVT/io/CAN.hpp>
#include <EVT/io/CANopen.hpp>

#include <EVT/io/GPIO.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>

//Temp
#define MAX_PACK_VOLTAGE 12000//in millivolts
#define MIN_PACK_VOLTAGE -1   //in millivolts
#define MAX_CELL_VOLTAGE 4200 //in millivolts
#define MIN_CELL_VOLTAGE -1   //in millivolts
#define MAX_TEMPERATURE 90.0
#define MIN_TEMPERATURE -1

#define CHECK_IN_RANGE(V, H, L) ((L) < (V) && (V) < (H))

#define BAD_PACK_VOLTAGE 0x1
#define BAD_MAX_CELL_VOLTAGE 0x2
#define BAD_MIN_CELL_VOLTAGE 0x4
#define BAD_MAX_TEMP 0x8
#define BAD_MIN_TEMP 0x10
#define BAD_FAULT_STATE 0x20

#define CHARGER_STATUS_CAN_ID 0x18FF50E5


/**
 * https://elconchargers.com/?page_id=98
 * https://www.diyelectriccar.com/threads/setting-up-can-bus-for-elcon-charger.202419/
 *
 * The page this information came from is no longer available...
 * MANUFACTURER MESSAGE ABOUT CAN
 *
 * 1.8KW and 3.3KW chargers use 250Kbps baud rate and 29-bit extended frame CAN ID.  6.6KW chargers use 500Kbps.
 * The charger expects every second to receive a CAN message from the BMS with CAN ID 1806E5F4 and
 * 8 data bytes with voltage and current required.  For example 98V and 16A would be 980 = 03D4 hex and 0160 = 00A0 hex
 * so the 8 data bytes would be 03D4 00A0 0000 0000.  If the charger does not receive a valid CAN message in 5 seconds,
 * it stops charging with a green blinking LED.  It starts charging again when it gets a valid CAN message with a red
 * blinking LED.
 *
 * The charger sends out every second a status message with CAN ID 18FF50E5 with voltage, current and status information.
 *
 * Up to four Elcon PFC chargers can be on the same CAN bus with CAN IDs of E5, E7, E8 and E9.
 * A 120 ohm termination resistor is required between CAN-L and CAN-H.
 */

class ChargeController {
public:
    enum class ControllerStates {
        NO_BATTERY,
        CONNECTED,
        CHARGING,
        STANDBY,
        FAULT
    };

    ChargeController(BMSManager& bms, LCDDisplay& display, IO::CAN& can);

    /**
     * Initialize the submodules of the Charge Controller
     */
    void init();

    /**
     * Process the Charge Controller state and update accordingly
     */
    void process();

    /**
     * Check the status of the BMS and if all the measurements are in spec
     *
     * @return 0 if OK. Otherwise there is an error
     */
    uint8_t checkBMS();

    /**
     * Check if the BMS is in a charging state
     *
     * @return boolean on whether the BMS is in a charging state
     */
    bool isCharging() { return state == ControllerStates::CHARGING; };
    bool hasFault() { return state == ControllerStates::FAULT; };

    /**
     * Start charging if the BMS is ready and the button is pressed
     */
    void startCharging();

    /**
     * Stops charging if the BMS is charging
     */
    void stopCharging();

    /**
     * Send the required message to the charger to tell it to keep charging.
     */
    void sendChargerMessage();

    /**
      * Set's the charge controller voltages on the display
      * @param voltage
      * @param current
      */
    void setDisplayChargerValues(uint16_t voltage, uint16_t current);

private:
    /**
     * Monitor the BMS when it is in a no battery state
     */
    void noBatteryState();

    /**
     * Monitor the BMS when it is connected
     */
    void connectedState();

    /**
     * Monitor the BMS as it charges
     */
    void chargingState();

    /**
     * Monitor the BMS while it waits for user input to start charging
     */
    void standbyState();

    /**
     * Monitor the BMS after a fault occurs, until the fault is cleared
     */
    void faultState();

    BMSManager& bms;
    LCDDisplay& display;
    IO::CAN& can;

    ControllerStates state = ControllerStates::NO_BATTERY;
    bool changedState = true;

    static constexpr IO::GPIO::State RELAY_ON = IO::GPIO::State::HIGH;
    static constexpr IO::GPIO::State RELAY_OFF = IO::GPIO::State::LOW;
};

#endif// CHARGE_CONTROLLER_H
