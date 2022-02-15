//
// Created by matth on 10/2/2021.
//

#ifndef EXAMPLE_BMSMANAGER_H
#define EXAMPLE_BMSMANAGER_H

#include <EVT/io/CAN.hpp>

namespace IO = EVT::core::IO;
class BMSManager {
private:
    IO::CAN* can = nullptr;
    uint8_t address;
public:
    BMSManager(IO::CAN& can, uint8_t address);
    int getBattVoltage();
    int getBattTemperature();
    bool isConnected();
    bool faultDetected();
};


#endif //EXAMPLE_BMSMANAGER_H
