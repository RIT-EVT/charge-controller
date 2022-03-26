//
// Created by Matthew Heller on 9/30/2021.
//

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/CAN.hpp>
#include <EVT/utils/time.hpp>

#include "BMSManager.h"

namespace IO = EVT::core::IO;
namespace time = EVT::core::time;

#define MAX_VOLTAGE 12
#define MAX_TEMPERATURE 90.0

constexpr IO::Pin RELAY_CTL_PIN = IO::Pin::PA_1;
constexpr IO::Pin LED_PIN = IO::Pin::PA_2;
constexpr IO::Pin ON_OFF_BUTTON_PIN = IO::Pin::PA_3;
constexpr IO::Pin LCD_A0_PIN = IO::Pin::PA_4;
//IO pins
constexpr IO::Pin RELAY_CTL_PIN = IO::Pin::PB_0;
constexpr IO::Pin LED_PIN = IO::Pin::PA_10;
constexpr IO::Pin STANDBY_BUTTON_PIN = IO::Pin::PA_2;
constexpr IO::Pin START_BUTTON_PIN = IO::Pin::PA_7;
constexpr IO::Pin BATTERY_1_OK = IO::Pin::PA_9;
constexpr IO::Pin BATTERY_2_OK = IO::Pin::PA_8;

//LCD pins
constexpr IO::Pin LCD_A0_PIN = IO::Pin::PA_6;
constexpr IO::Pin LCD_RST_PIN = IO::Pin::PA_1;
constexpr IO::Pin SPI_CS_PIN = IO::Pin::PA_3;
constexpr IO::Pin SPI_MOSI_PIN = IO::Pin::PB_5;
constexpr IO::Pin SPI_CLK_PIN = IO::Pin::PA_5;

//UART pins
constexpr IO::Pin UART_RX_PIN = IO::Pin::PB_7;
constexpr IO::Pin UART_TX_PIN = IO::Pin::PB_6;

//CAN pins
constexpr IO::Pin CAN_RX_PIN = IO::Pin::PA_11;
constexpr IO::Pin CAN_TX_PIN = IO::Pin::PA_12;

constexpr IO::Pin UART_RX_PIN = IO::Pin::PA_10;
constexpr IO::Pin UART_TX_PIN = IO::Pin::PA_11;

constexpr IO::Pin CAN_RX_PIN = IO::Pin::PC_4;
constexpr IO::Pin CAN_TX_PIN = IO::Pin::PC_5;

void disconnectBattery(){

}

bool checkBMS(BMSManager bms){
    bool status = true;
    int temperature = bms.getBattTemperature();
    int voltage = bms.getBattVoltage();

    if(temperature > MAX_TEMPERATURE){
        disconnectBattery();
        status=false;
    }
    if(voltage > MAX_VOLTAGE){
        disconnectBattery();
        status=false;
    }
    if(bms.faultDetected()){
        disconnectBattery();
        status=false;
    }
    return status;
}

int main(){
    IO::init();

    constexpr uint8_t BMS_CAN_ADDRESS = 0x00;

    //Pin initialization
    IO::UART& uart = IO::getUART<UART_TX_PIN, UART_RX_PIN>(9600);
    IO::CAN& can = IO::getCAN<CAN_TX_PIN, CAN_RX_PIN>();
    IO::GPIO& relayControl = IO::getGPIO<RELAY_CTL_PIN>();
    IO::GPIO& statusLED = IO::getGPIO<LED_PIN>();
    IO::GPIO& LCDRegisterSEL = IO::getGPIO<LCD_A0_PIN>();

    //GPIO pins
    IO::GPIO& relayControl = IO::getGPIO<RELAY_CTL_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO& statusLED = IO::getGPIO<LED_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO& standbyButton = IO::getGPIO<STANDBY_BUTTON_PIN>(IO::GPIO::Direction::INPUT);
    IO::GPIO& startButton = IO::getGPIO<START_BUTTON_PIN>(IO::GPIO::Direction::INPUT);
    IO::GPIO& batt1OK = IO::getGPIO<BATTERY_1_OK>(IO::GPIO::Direction::INPUT);
    IO::GPIO& batt2OK = IO::getGPIO<BATTERY_2_OK>(IO::GPIO::Direction::INPUT);

    //LCD
    IO::GPIO& LCDRegisterSEL = IO::getGPIO<LCD_A0_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO& LCDReset = IO::getGPIO<LCD_RST_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO* devices[] = {&IO::getGPIO<SPI_CS_PIN>(IO::GPIO::Direction::OUTPUT)};
    devices[0]->writePin(IO::GPIO::State::HIGH);

    IO::SPI& spi = IO::getSPI<SPI_CLK_PIN, SPI_MOSI_PIN>(devices, 1);

    //charge controller module instantiation
    BMSManager bms = BMSManager(can, BMS_CAN_ADDRESS);

    while (1){
        int temperature = bms.getBattTemperature();
        int voltage = bms.getBattVoltage();




        time::wait(50);
    }

}

