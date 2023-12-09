
#include <charge_controller/dev/ControllerUI.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>
#include <cstdio>

LCDDisplay::LCDDisplay(IO::GPIO& reg_select, IO::GPIO& reset, IO::SPI& spi, ControllerModel& model) : lcd(DEV::LCD(reg_select, reset, spi, 12, 3)), model(model) {}

void LCDDisplay::init() {
    //TODO change this init method to display the correct things
    lcd.initLCD();
    lcd.clearLCD();
    lcd.setDefaultSections(MAIN_SCREEN_SECTION_TITLES);

    // Splash Screen
    lcd.displaySectionHeaders();
}

void LCDDisplay::setChargeControllerStatus(const char* str) {
    chargeControllerStatus = str;
}

void LCDDisplay::display() {
    ControllerModel::Page newPage = model.getPage();
    switch(newPage) {
    case ControllerModel::Page::MAINSCREEN:
        {
            if (page != ControllerModel::Page::MAINSCREEN) {
                lcd.clearLCD();
                lcd.setNewSections(8, 4, MAIN_SCREEN_SECTION_TITLES);
                lcd.displaySectionHeaders();
                page = ControllerModel::Page::MAINSCREEN;
            }
            //B1 Status
            lcd.setTextForSection(0, batteryOneStatus);
            //B2 Status
            lcd.setTextForSection(1, batteryTwoStatus);
            //B1 Voltage
            char batteryOneVolt[16] = {0};
            std::sprintf(batteryOneVolt, "%d V", batteryMinVoltages[0]);
            lcd.setTextForSection(2, batteryOneVolt);
            //B2 Voltage
            char batteryTwoVolt[16] = {0};
            std::sprintf(batteryTwoVolt, "%d V", batteryMinVoltages[1]);
            lcd.setTextForSection(3, batteryTwoVolt);
            //B1 Min T
            char bat1MinTemp[16] = {0};
            std::sprintf(bat1MinTemp, "%d C", batteryMinTemps[0]);
            lcd.setTextForSection(4, bat1MinTemp);
            //B2 Min T
            char bat2MinTemp[16] = {0};
            std::sprintf(bat2MinTemp, "%d C", batteryMinTemps[1]);
            lcd.setTextForSection(5, bat2MinTemp);
            //B1 Max T
            char bat1MaxTemp[16] = {0};
            std::sprintf(bat1MaxTemp, "%d C", batteryMaxTemps[0]);
            lcd.setTextForSection(6, bat1MaxTemp);
            //B2 Max T
            char bat2MaxTemp[16] = {0};
            std::sprintf(bat2MaxTemp, "%d C", batteryMaxTemps[1]);
            lcd.setTextForSection(7, bat2MaxTemp);
            break;
        }
        case ControllerModel::Page::SETTINGSCREEN:
        {
            if (page != ControllerModel::Page::SETTINGSCREEN) {
                lcd.clearLCD();
                lcd.setNewSections(8, 4, SETTING_SCREEN_SECTION_TITLES);
                lcd.displaySectionHeaders();
                page = ControllerModel::Page::SETTINGSCREEN;
            }
            //CC Status
            lcd.setTextForSection(0, chargeControllerStatus);
            //Charge %
            char percentage[16] = {0};
            std::sprintf(percentage, "%d.%02d", chargePercentage / 100, chargePercentage % 100);
            lcd.setTextForSection(1, percentage);

            //C Voltage -> The voltage the charge controller is returning
            char chargerVoltage[16] = {0};
            std::sprintf(chargerVoltage, "%d.%d V", chargeControllerVoltage / 10, chargeControllerVoltage % 10);
            lcd.setTextForSection(2, chargerVoltage);

            //C Current -> The current the charge controller is returning
            char chargerCurrent[16] = {0};
            std::sprintf(chargerCurrent, "%d.%d A", chargeControllerCurrent / 10, chargeControllerCurrent % 10);
            lcd.setTextForSection(3, chargerCurrent);

            char voltageSetting[16] = {0};
            std::sprintf(voltageSetting, " %d V", model.getUnsavedVoltage());

            char currentSetting[16] = {0};
            std::sprintf(currentSetting, " %d A", model.getUnsavedCurrent());

            char save[16] = " Save";

            char quit[16] = " Quit";

            //Determining which option is selected
            //The current selected setting will have a '>' in the first character of the string instead of a ' '
            if (model.getState() == ControllerModel::SETTINGSELECT) {
                switch(model.getSetting()) {
                case ControllerModel::VOLTAGE:
                    voltageSetting[0] = '>';
                    break;
                case ControllerModel::CURRENT:
                    currentSetting[0] = '>';
                    break;
                case ControllerModel::SAVE:
                    save[0] = '>';
                    break;
                case ControllerModel::QUIT:
                    quit[0] = '>';
                    break;
                }
            }
            //S Voltage -> The voltage that the charge controller is being commanded to run at
            lcd.setTextForSection(4, voltageSetting);
            //S Current -> The current that the charge controller is being commanded to run at
            lcd.setTextForSection(5, currentSetting);
            //Save
            lcd.setTextForSection(6,save);
            //Quit
            lcd.setTextForSection(7, quit);
            break;
        }
    }

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
