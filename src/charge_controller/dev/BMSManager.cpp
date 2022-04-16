#include <charge_controller/dev/BMSManager.hpp>
int BMSManager::getBattVoltage() { return 0; }

int BMSManager::getBattTemperature() { return 0; }

bool BMSManager::isConnected() { return false; }

bool BMSManager::faultDetected() { return false; }

BMSManager::BMSManager(IO::CAN &can, uint8_t address) { this->can = &can; }

int BMSManager::getStateOfCharge() { return 0; }
