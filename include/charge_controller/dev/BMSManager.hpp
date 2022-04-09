#ifndef EXAMPLE_BMSMANAGER_HPP
#define EXAMPLE_BMSMANAGER_HPP

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
    int getStateOfCharge();
    bool isConnected();
    bool faultDetected();
};


#endif //EXAMPLE_BMSMANAGER_HPP
