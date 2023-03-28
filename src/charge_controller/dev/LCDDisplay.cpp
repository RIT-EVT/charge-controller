
#include <charge_controller/dev/LCDDisplay.hpp>
#include <cstdio>


LCDDisplay::LCDDisplay(IO::GPIO& reg_select, IO::GPIO& reset, IO::SPI& spi) :
lcd(DEV::LCD(reg_select, reset, spi, 12, 3, 2)) { }

void LCDDisplay::init() {
    lcd.initLCD();
    lcd.clearLCD();
    lcd.setDefaultSections(SECTION_TITLES);
    showSplashScreen();
}

void LCDDisplay::showSplashScreen() {
    lcd.clearLCD();
    lcd.setEntireScreenBitMap(evtBitMap);
}

void LCDDisplay::showSections() {
    lcd.clearLCD();
    lcd.displaySectionHeaders();
}

void LCDDisplay::setChargeControllerStatus(const char *str) {
    chargeControllerStatus = str;
}

void LCDDisplay::display() {
    lcd.setTextForSection(0, batteryOneStatus);

    lcd.setTextForSection(1, chargeControllerStatus);

    lcd.setTextForSection(2, batteryTwoStatus);

    char batteryOneVolt[32];
    std::sprintf(batteryOneVolt, "%d V", batteryOneVoltage);
    lcd.setTextForSection(3, batteryOneVolt);
    char percentage[32];
    std::sprintf(percentage, "%d.%02d", chargePercentage / 100, chargePercentage % 100);
    lcd.setTextForSection(4, percentage);

    char batteryTwoVolt[32];
    std::sprintf(batteryTwoVolt, "%d V", batteryTwoVoltage);
    lcd.setTextForSection(5, batteryTwoVolt);

    char bat1MinTemp[32];
    std::sprintf(bat1MinTemp, "%d C", batteryOneMinTemp);
    lcd.setTextForSection(6, bat1MinTemp);

    char chargerVoltage[32];
    std::sprintf(chargerVoltage, "%d V", chargeControllerVoltage);
    lcd.setTextForSection(7, chargerVoltage);

    char bat2MinTemp[32];
    std::sprintf(bat2MinTemp, "%d C", batteryTwoMinTemp);
    lcd.setTextForSection(8, bat2MinTemp);

    char bat1MaxTemp[32];
    std::sprintf(bat1MaxTemp, "%d C", batteryOneMaxTemp);
    lcd.setTextForSection(9, bat1MaxTemp);

    char currentDisplay[32];
    std::sprintf(currentDisplay, "%d A", chargeControllerCurrent);
    lcd.setTextForSection(10, currentDisplay);

    char bat2MaxTemp[32];
    std::sprintf(bat2MaxTemp, "%d C", batteryTwoMaxTemp);
    lcd.setTextForSection(11, bat2MaxTemp);
}
