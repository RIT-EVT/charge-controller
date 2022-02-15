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


    IO::UART& uart = IO::getUART<IO::Pin::PA_11, IO::Pin::PA_10>(9600);
    IO::CAN& can = IO::getCAN<IO::Pin::PA_12, IO::Pin::PA_11>();
    IO::GPIO& relayControl = IO::getGPIO<EVT::core::IO::Pin::PA_1>();
    IO::GPIO& statusLED = IO::getGPIO<EVT::core::IO::Pin::PA_2>();
    IO::GPIO& LCDRegisterSEL = IO::getGPIO<EVT::core::IO::Pin::PA_4>();

    BMSManager bms = BMSManager(can, BMS_CAN_ADDRESS);

    while (1){
        int temperature = bms.getBattTemperature();
        int voltage = bms.getBattVoltage();




        time::wait(50);
    }

}

