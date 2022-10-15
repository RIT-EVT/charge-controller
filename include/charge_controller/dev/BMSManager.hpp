#ifndef EXAMPLE_BMSMANAGER_HPP
#define EXAMPLE_BMSMANAGER_HPP

#include <EVT/io/CAN.hpp>
#include <charge_controller/Logger.h>
#include <EVT/io/GPIO.hpp>

namespace IO = EVT::core::IO;
class BMSManager {
    static constexpr uint32_t TOTAL_VOLTAGE_ID = 0x2101;
    static constexpr uint32_t STATE_ID = 0x2102;
public:
    BMSManager(IO::CAN& can, IO::GPIO& bmsOK1, IO::GPIO& bmsOK2);

    enum class BMSState {
        /// When the BMS is powered on
        START = 0,
        /// When the BMS fails startup sequence
        INITIALIZATION_ERROR = 1,
        /// When the system is waiting for settings to be sent to the BMS
        FACTORY_INIT = 2,
        /// When the BMS is actively sending settings over to the BQ
        TRANSFER_SETTINGS = 3,
        /// When the BMS is ready for charging / discharging
        SYSTEM_READY = 4,
        /// When the system is running in a low power mode
        DEEP_SLEEP = 5,
        /// When a fault is detected during normal operation
        UNSAFE_CONDITIONS_ERROR = 6,
        /// When the BMS is on the bike and delivering power
        POWER_DELIVERY = 7,
        /// When the BMS is handling charging the battery pack
        CHARGING = 8
    };

    void updateBMSData(IO::CANMessage& message);
    int getBattVoltage() {return millivolts;};
    int getBattTemperature() { return 0; };
    BMSState getBMSState(){ return state; };
    int getStateOfCharge();
    bool isConnected();
    bool faultDetected();

private:
    IO::CAN* can = nullptr;
    IO::GPIO& bmsOK1;
    IO::GPIO& bmsOK2;
    uint32_t millivolts = 0;
    uint32_t temperature = 5;
    BMSState state = BMSState::START;
};

#endif// EXAMPLE_BMSMANAGER_HPP
