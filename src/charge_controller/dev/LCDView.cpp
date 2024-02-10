#include <charge_controller/dev/LCDView.hpp>

CC::LCDView::LCDView(IO::GPIO& reg_select, IO::GPIO& reset, IO::SPI& spi, UIModel& model) : lcd(DEV::LCD(reg_select, reset, spi, 9, 3)), model(model) {}

void CC::LCDView::init() {
    lcd.initLCD();
    lcd.clearLCD();
    lcd.setDefaultSections(MAIN_SCREEN_SECTION_TITLES);

    // Splash Screen
    lcd.displaySectionHeaders();
}

void CC::LCDView::setChargeControllerStatus(const char* str) {
    chargeControllerStatus = str;
}

void CC::LCDView::display() {
    UIModel::Page newPage = model.getPage();
    switch (newPage) {
    /**
     * Each of these cases must be scoped (enclosed in {}) because there are variable declarations inside them.
     * If they are not scoped, these variables could possibly be accessed undeclared, which would be BAD.
     */
    case UIModel::Page::MAIN: {
        if (page != newPage) {
            lcd.clearLCD();
            lcd.setNewSections(9, 3, MAIN_SCREEN_SECTION_TITLES);
            lcd.displaySectionHeaders();
            page = UIModel::Page::MAIN;
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
    case UIModel::Page::SETTINGS: {
        if (page != newPage) {
            lcd.clearLCD();
            lcd.setNewSections(9, 3, SETTING_SCREEN_SECTION_TITLES);
            lcd.displaySectionHeaders();
            page = UIModel::Page::SETTINGS;
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

        char reset[16] = " Reset";

        //If there is an unsaved current or voltage, it will be represented by a * next to that setting.
        if (model.getUnsavedCurrent() != model.getSavedCurrent()) {
            currentSetting[0] = UNSAVED_PREFIX_CHAR;
        }
        if (model.getUnsavedVoltage() != model.getSavedVoltage()) {
            voltageSetting[0] = UNSAVED_PREFIX_CHAR;
        }

        //When the current or voltage is being modified, it will be represented by a < next to that setting
        if (model.getState() == UIModel::CURRENT_SELECT) {
            currentSetting[0] = EDITING_PREFIX_CHAR;
        } else if (model.getState() == UIModel::VOLTAGE_SELECT) {
            voltageSetting[0] = EDITING_PREFIX_CHAR;
        }
        //Determining which option is selected
        //The current selected setting will have a '>' in the first character of the string instead of a ' '
        if (model.getState() == UIModel::SETTING_SELECT) {
            switch (model.getSelectedSetting()) {
            case UIModel::VOLTAGE:
                voltageSetting[0] = SELECTED_PREFIX_CHAR;
                break;
            case UIModel::CURRENT:
                currentSetting[0] = SELECTED_PREFIX_CHAR;
                break;
            case UIModel::SAVE:
                save[0] = SELECTED_PREFIX_CHAR;
                break;
            case UIModel::QUIT:
                quit[0] = SELECTED_PREFIX_CHAR;
                break;
            case UIModel::RESET:
                reset[0] = SELECTED_PREFIX_CHAR;
                break;
            }
        }
        //S Voltage -> The voltage that the charge controller is being commanded to run at
        lcd.setTextForSection(4, voltageSetting);
        //S Current -> The current that the charge controller is being commanded to run at
        lcd.setTextForSection(5, currentSetting);

        //Blanks out the save, quit, and reset options unless you are clicked into the screen
        //Suggested by Luke (Integration) to avoid people being able to mess with the settings as easily
        //Also provides visual feedback that the user has the settings screen selected
        if (model.getState() != UIModel::State::PAGE_SELECT) {
            //Save
            lcd.setTextForSection(6, save);
            //Quit
            lcd.setTextForSection(7, quit);
            //Reset
            lcd.setTextForSection(8, reset);
        } else {
            lcd.setTextForSection(6, "");
            lcd.setTextForSection(7, "");
            lcd.setTextForSection(8, "");
        }
        break;
    }
    }
}

void CC::LCDView::setChargerVoltage(uint16_t voltage) {
    chargeControllerVoltage = voltage;
}

void CC::LCDView::setChargerCurrent(uint16_t current) {
    chargeControllerCurrent = current;
}

void CC::LCDView::setBatteryStatus(BMSManager::BMSStatus status, uint8_t index) {
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

void CC::LCDView::setMinCellVoltage(int16_t cellVoltage, uint8_t index) {
    if (index == 0) {
        batteryMinVoltages[0] = cellVoltage;
    } else if (index == 1) {
        batteryMinVoltages[1] = cellVoltage;
    }
}

void CC::LCDView::setMaxCellVoltage(int16_t cellVoltage, uint8_t index) {
    if (index == 0) {
        batteryMaxVoltages[0] = cellVoltage;
    } else if (index == 1) {
        batteryMaxVoltages[1] = cellVoltage;
    }
}

void CC::LCDView::setMinTemp(int16_t temp, uint8_t index) {
    if (index == 0) {
        batteryMinTemps[0] = temp;
    } else if (index == 1) {
        batteryMinTemps[1] = temp;
    }
}

void CC::LCDView::setMaxTemp(int16_t temp, uint8_t index) {
    if (index == 0) {
        batteryMaxTemps[0] = temp;
    } else if (index == 1) {
        batteryMaxTemps[1] = temp;
    }
}
