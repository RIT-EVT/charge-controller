#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <EVT/dev/LCD.hpp>
#include <EVT/io/SPI.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <cstdint>

namespace IO = EVT::core::IO;
namespace DEV = EVT::core::DEV;

class LCDDisplay {
public:
    /**
     * Initializer for the LCD Display class.
     *
     * @param[in] regSelect Register select pin
     * @param[in] reset Reset pin
     * @param[in] spi SPI class for communication
     */
    LCDDisplay(IO::GPIO& reg_select, IO::GPIO& reset, IO::SPI& spi);

    /**
     * Initializes the LCD driver and displays the splash image.
     */
    void init();

    /**
     * The display loop that updates the section headers.
     */
    void display();

    /**
     * Set the charge controller status display
     */
    void setChargeControllerStatus(const char* str);

    void setChargerVoltage(uint16_t voltage);
    void setChargerCurrent(uint16_t current);

    /**
     *
     */
    void setBatteryStatus(BMSManager::BMSStatus status, uint8_t index);

    /**
    *
    */
    void setMinCellVoltage(int16_t cellVoltage, uint8_t index);

    /**
    *
    */
    void setMaxCellVoltage(int16_t cellVoltage, uint8_t index);

    /**
    *
    */
    void setMinTemp(int16_t temp, uint8_t index);

    /**
    *
    */
    void setMaxTemp(int16_t temp, uint8_t index);

private:
    /**
     * The internal LCD driver.
     */
    DEV::LCD lcd;

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
    /** The current battery one voltage */
    int16_t batteryOneVoltage = 0;
    /** The current battery two voltage */
    int16_t batteryTwoVoltage = 0;

    /** The minimum temperature for battery one */
    int16_t batteryOneMinTemp = 0;
    /** The maximum temperature for battery one */
    int16_t batteryOneMaxTemp = 0;
    /** The minimum temperature for battery two */
    int16_t batteryTwoMinTemp = 0;
    /** The maximum temperature for battery two */
    int16_t batteryTwoMaxTemp = 0;
    /** How charged the batteries currently are. */
    const uint8_t chargePercentage = 0;

    /**
     * The 9 section headers to be displayed.
     */
    static constexpr char* SECTION_TITLES[12]{
        (char*) "B1 Status",
        (char*) "CC Status",
        (char*) "B2 Status",
        (char*) "B1 Voltage",
        (char*) "Charge %",
        (char*) "B2 Voltage",
        (char*) "B1 Min T",
        (char*) "C Voltage",
        (char*) "B2 Min T",
        (char*) "B1 Max T",
        (char*) "C Current",
        (char*) "B2 Max T",
    };

    /**
     * The EVT logo used for the splash screen.
     */
    //    unsigned char evtBitMap[1024] = {
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0x3F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x7F,
    //        0x7F,
    //        0x7F,
    //        0x7F,
    //        0x7F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x3F,
    //        0x03,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x80,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF8,
    //        0xFC,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xF7,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0xC0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x0F,
    //        0x01,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x18,
    //        0x1F,
    //        0x1F,
    //        0x9F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x1F,
    //        0x07,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x18,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0xDF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x7F,
    //        0x3F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x9F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x7F,
    //        0x1F,
    //        0x01,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0xC0,
    //        0xF8,
    //        0xFE,
    //        0xFF,
    //        0xFF,
    //        0x3F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFB,
    //        0xF8,
    //        0xF8,
    //        0x78,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0xC0,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xFF,
    //        0xFF,
    //        0x3F,
    //        0x07,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0xC0,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xF3,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xC0,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x03,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x7F,
    //        0x1F,
    //        0x0F,
    //        0x07,
    //        0x01,
    //        0x00,
    //        0x00,
    //        0x80,
    //        0xC0,
    //        0xE0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xFC,
    //        0xFF,
    //        0xFF,
    //        0x0F,
    //        0x07,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0xF0,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xF8,
    //        0xFC,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFE,
    //        0xFE,
    //        0xFE,
    //        0xFE,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x1F,
    //        0x07,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x18,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x1F,
    //        0x9F,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x03,
    //        0x03,
    //        0x01,
    //        0x01,
    //        0x00,
    //        0xC0,
    //        0xE0,
    //        0xE0,
    //        0xF0,
    //        0xFC,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0x7F,
    //        0x3F,
    //        0x03,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x00,
    //        0x80,
    //        0xE0,
    //        0xFE,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xF7,
    //        0xF1,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF8,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xF8,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF8,
    //        0xFC,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xF7,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF0,
    //        0xF8,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //        0xFF,
    //    };
};

#endif// LCDDISPLAY_H
