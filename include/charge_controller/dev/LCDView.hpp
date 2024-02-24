#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <EVT/dev/LCD.hpp>
#include <EVT/io/SPI.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <charge_controller/dev/UIModel.hpp>
#include <cstdint>
#include <cstdio>

#define UNSAVED_PREFIX_CHAR '*'
#define EDITING_PREFIX_CHAR '<'
#define SELECTED_PREFIX_CHAR '>'

namespace IO = EVT::core::IO;
namespace DEV = EVT::core::DEV;

namespace CC {

/**
 * Outputs all the values from the ChargeController & UIModel classes to the LCD display
 * Acts as the view in a Model-View-Controller (MVC) design pattern where
 * UIModel is the model,
 * LCDView is the view, and
 * UIController is the controller.
 */
class LCDView {
public:
    /**
     * Initializer for the LCDView class.
     *
     * @param[in] regSelect Register select pin
     * @param[in] reset Reset pin
     * @param[in] spi SPI class for communication
     */
    LCDView(IO::GPIO& reg_select, IO::GPIO& reset, IO::SPI& spi, UIModel& model);

    /**
     * Initializes the LCD driver
     */
    void init();

    /**
     * The display loop that updates the section headers.
     */
    void display();

    /**
     * Set the charge controller status section.
     *
     * @param str the status string to display.
     */
    void setChargeControllerStatus(const char* str);

    /**
     * Set the "voltage" display section.
     *
     * @param voltage the voltage to display.
     */
    void setChargerVoltage(uint16_t voltage);

    /**
     * Set the "current" display section.
     *
     * @param current the "current" to display.
     */
    void setChargerCurrent(uint16_t current);

    /**
     * Set the status of the battery at the given index.
     * Will display in either the BM1 (BMS 1) column if index is 0
     * or BM2 (BMS 2).
     *
     * @param status the status to display for the BMS at index.
     * @param index the index of the bms to display too (0 - 1).
     */
    void setBatteryStatus(BMSManager::BMSStatus status, uint8_t index);

    /**
     * Sets the display for the BMS at the given index's minimum cell voltage.
     *
     * @param cellVoltage the cell voltage to display.
     * @param index the index of the bms.
     */
    void setMinCellVoltage(int16_t cellVoltage, uint8_t index);

    /**
     * Sets the display for the BMS at the given index's maximum cell voltage.
     *
     * @param cellVoltage the cell voltage to display.
     * @param index the index of the bms.
     */
    void setMaxCellVoltage(int16_t cellVoltage, uint8_t index);

    /**
     * Sets the display for the BMS at the given index's minimum temperature.
     *
     * @param temp the temperature to display.
     * @param index the index of the bms.
     */
    void setMinTemp(int16_t temp, uint8_t index);

    /**
     * Sets the display for the BMS at the given index's maximum temperature.
     *
     * @param temp the temperature to display.
     * @param index the index of the bms.
     */
    void setMaxTemp(int16_t temp, uint8_t index);

private:
    /**
     * The internal LCD driver.
     */
    DEV::LCD lcd;

    /**
     * The model reference
     */
    UIModel& model;

    /** The current status of the charge controller */
    const char* chargeControllerStatus = "NULL";
    /** The current status for battery  */
    const char* batteryOneStatus = "NO CONN";
    /** The current charge controller status */
    const char* batteryTwoStatus = "NO CONN";
    /** The voltage that is being supplied */
    uint16_t chargeControllerVoltage = 0;
    /** The current that is being supplied */
    uint16_t chargeControllerCurrent = 0;
    /** The current page that the display is on */
    UIModel::Page page = UIModel::Page::MAIN;

    int16_t batteryMinVoltages[2] = {};
    int16_t batteryMaxVoltages[2] = {};

    int16_t batteryMinTemps[2] = {};
    int16_t batteryMaxTemps[2] = {};

    /** How charged the batteries currently are. */
    const uint8_t chargePercentage = 0;

    /**
     * The 8 section headers for the main screen
     */
    static constexpr char* MAIN_SCREEN_SECTION_TITLES[9]{
        (char*) "B1 Status",
        (char*) "B2 Status",
        (char*) "B1 Voltage",
        (char*) "B2 Voltage",
        (char*) "B1 Min T",
        (char*) "B2 Min T",
        (char*) "B1 Max T",
        (char*) "B2 Max T",
        (char*) "Page 1/2"};

    /**
     * The 8 section headers for the setting screen
     */
    static constexpr char* SETTING_SCREEN_SECTION_TITLES[9]{
        (char*) "CC Status",
        (char*) "Charge %",
        (char*) "C Voltage",
        (char*) "C Current",
        (char*) "S Voltage",
        (char*) "S Current",
        //The next three section headers are left blank because they are selectable.
        //We leave their headers blank, and write their header (save,quit,reset) in
        //their 'text' section, so we are able to add a '>' in front if they are selected.
        (char*) "",//Save
        (char*) "",//Quit
        (char*) "" //Reset
    };
};

}//namespace CC

#endif// LCDDISPLAY_H
