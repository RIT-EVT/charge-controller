#ifndef CHARGE_CONTROLLER_H
#define CHARGE_CONTROLLER_H

#include <EVT/io/CAN.hpp>
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

class ChargeController {
public:
    enum class ControllerStates {
        NO_BATTERY,
        CONNECTED,
        CHARGING,
        STANDBY,
        FAULT
    };
    ChargeController(BMSManager bms, LCDDisplay& display, IO::GPIO& relay);
    void init();
    void loop();

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
    void stopCharging();

private:
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

    BMSManager bms;
    LCDDisplay& display;
    IO::GPIO& relay;

    ControllerStates state = ControllerStates::NO_BATTERY;
    bool changedState = true;

    static constexpr IO::GPIO::State RELAY_ON = IO::GPIO::State::HIGH;
    static constexpr IO::GPIO::State RELAY_OFF = IO::GPIO::State::LOW;
};

#endif// CHARGE_CONTROLLER_H
