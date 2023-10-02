
#include <charge_controller/dev/LCDDisplay.hpp>
#include <cstdio>

LCDDisplay::LCDDisplay(IO::GPIO& reg_select, IO::GPIO& reset, IO::SPI& spi) : lcd(DEV::LCD(reg_select, reset, spi, 12, 3)) {}

void LCDDisplay::init() {
    lcd.initLCD();
    lcd.clearLCD();
    lcd.setDefaultSections(SECTION_TITLES);

    // Splash Screen
    lcd.displaySectionHeaders();
}

void LCDDisplay::setChargeControllerStatus(const char* str) {
    chargeControllerStatus = str;
}

void LCDDisplay::display() {
    lcd.setTextForSection(0, batteryOneStatus);

    lcd.setTextForSection(1, chargeControllerStatus);

    lcd.setTextForSection(2, batteryTwoStatus);

    char batteryOneVolt[16];
    std::sprintf(batteryOneVolt, "%d V", batteryMinVoltages[0]);
    lcd.setTextForSection(3, batteryOneVolt);
    char percentage[16];
    std::sprintf(percentage, "%d.%02d", chargePercentage / 100, chargePercentage % 100);
    lcd.setTextForSection(4, percentage);

    char batteryTwoVolt[16];
    std::sprintf(batteryTwoVolt, "%d V", batteryMinVoltages[1]);
    lcd.setTextForSection(5, batteryTwoVolt);

    char bat1MinTemp[16];
    std::sprintf(bat1MinTemp, "%d C", batteryMinTemps[0]);
    lcd.setTextForSection(6, bat1MinTemp);

    char chargerVoltage[16];
    std::sprintf(chargerVoltage, "%d.%d V", chargeControllerVoltage / 10, chargeControllerVoltage % 10);
    lcd.setTextForSection(7, chargerVoltage);

    char bat2MinTemp[16];
    std::sprintf(bat2MinTemp, "%d C", batteryMinTemps[1]);
    lcd.setTextForSection(8, bat2MinTemp);

    char bat1MaxTemp[16];
    std::sprintf(bat1MaxTemp, "%d C", batteryMaxTemps[0]);
    lcd.setTextForSection(9, bat1MaxTemp);

    char currentDisplay[16];
    std::sprintf(currentDisplay, "%d.%d A", chargeControllerCurrent / 10, chargeControllerCurrent % 10);
    lcd.setTextForSection(10, currentDisplay);

    char bat2MaxTemp[16];
    std::sprintf(bat2MaxTemp, "%d C", batteryMaxTemps[1]);
    lcd.setTextForSection(11, bat2MaxTemp);
}

void LCDDisplay::setChargerVoltage(uint16_t voltage) {
    chargeControllerVoltage = voltage;
}

void LCDDisplay::setChargerCurrent(uint16_t current) {
    chargeControllerCurrent = current;
}

void LCDDisplay::setBatteryStatus(BMSManager::BMSStatus status, uint8_t index) {
    if (index == 0) {
        switch (status) {
        case BMSManager::BMSStatus::START:
            this->batteryOneStatus = "Start";
            break;
        case BMSManager::BMSStatus::INITIALIZATION_ERROR:
            this->batteryOneStatus = "Init Error";
            break;
        case BMSManager::BMSStatus::FACTORY_INIT:
            this->batteryOneStatus = "Fact Init";
            break;
        case BMSManager::BMSStatus::TRANSFER_SETTINGS:
            this->batteryOneStatus = "Transf Sett";
            break;
        case BMSManager::BMSStatus::SYSTEM_READY:
            this->batteryOneStatus = "Sys Ready";
            break;
        case BMSManager::BMSStatus::DEEP_SLEEP:
            this->batteryOneStatus = "Deep Sleep";
            break;
        case BMSManager::BMSStatus::UNSAFE_CONDITIONS_ERROR:
            this->batteryOneStatus = "UC Error";
            break;
        case BMSManager::BMSStatus::POWER_DELIVERY:
            this->batteryOneStatus = "Pow Deli";
            break;
        case BMSManager::BMSStatus::CHARGING:
            this->batteryOneStatus = "Charging";
            break;
        case BMSManager::BMSStatus::NOT_CONNECTED:
            this->batteryOneStatus = "No Conn";
            break;
        }
    } else if (index == 1) {
        switch (status) {
        case BMSManager::BMSStatus::START:
            this->batteryTwoStatus = "Start";
            break;
        case BMSManager::BMSStatus::INITIALIZATION_ERROR:
            this->batteryTwoStatus = "Init Error";
            break;
        case BMSManager::BMSStatus::FACTORY_INIT:
            this->batteryTwoStatus = "Fact Init";
            break;
        case BMSManager::BMSStatus::TRANSFER_SETTINGS:
            this->batteryTwoStatus = "Transf Sett";
            break;
        case BMSManager::BMSStatus::SYSTEM_READY:
            this->batteryTwoStatus = "Sys Ready";
            break;
        case BMSManager::BMSStatus::DEEP_SLEEP:
            this->batteryTwoStatus = "Deep Sleep";
            break;
        case BMSManager::BMSStatus::UNSAFE_CONDITIONS_ERROR:
            this->batteryTwoStatus = "UC Error";
            break;
        case BMSManager::BMSStatus::POWER_DELIVERY:
            this->batteryTwoStatus = "Pow Deli";
            break;
        case BMSManager::BMSStatus::CHARGING:
            this->batteryTwoStatus = "Charging";
            break;
        case BMSManager::BMSStatus::NOT_CONNECTED:
            this->batteryTwoStatus = "No Conn";
            break;
        }
    }
}

void LCDDisplay::setMinCellVoltage(int16_t cellVoltage, uint8_t index) {
    if (index == 0) {
        batteryMinVoltages[0] = cellVoltage;
    } else if (index == 1) {
        batteryMinVoltages[1] = cellVoltage;
    }
}

void LCDDisplay::setMaxCellVoltage(int16_t cellVoltage, uint8_t index) {
    if (index == 0) {
        batteryMaxVoltages[0] = cellVoltage;
    } else if (index == 1) {
        batteryMaxVoltages[1] = cellVoltage;
    }
}

void LCDDisplay::setMinTemp(int16_t temp, uint8_t index) {
    if (index == 0) {
        batteryMinTemps[0] = temp;
    } else if (index == 1) {
        batteryMinTemps[1] = temp;
    }
}

void LCDDisplay::setMaxTemp(int16_t temp, uint8_t index) {
    if (index == 0) {
        batteryMaxTemps[0] = temp;
    } else if (index == 1) {
        batteryMaxTemps[1] = temp;
    }
}
