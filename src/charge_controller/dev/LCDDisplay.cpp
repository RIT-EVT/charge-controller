
#include <charge_controller/dev/LCDDisplay.hpp>
#include <cstdio>

LCDDisplay::LCDDisplay(IO::GPIO& reg_select, IO::GPIO& reset, IO::SPI& spi) : lcd(DEV::LCD(reg_select, reset, spi)) { }

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

void LCDDisplay::display() {
    lcd.setTextForSection(0, status);

    char chargerVoltage[32];
    std::sprintf(chargerVoltage, "%d v", chargeControllerVoltage);
    lcd.setTextForSection(1, chargerVoltage);

    char currentDisplay[32];
    std::sprintf(currentDisplay, "%d A", chargeControllerCurrent);
    lcd.setTextForSection(2, currentDisplay);

    char batteryVolt[32];
    std::sprintf(batteryVolt, "%d v", batteryVoltage);
    lcd.setTextForSection(3, batteryVolt);

    char bat1MinTemp[32];
    std::sprintf(bat1MinTemp, "%d C", batteryOneMinTemp);
    lcd.setTextForSection(4, bat1MinTemp);

    char bat1MaxTemp[32];
    std::sprintf(bat1MaxTemp, "%d C", batteryOneMaxTemp);
    lcd.setTextForSection(5, bat1MaxTemp);

    char percentage[32];
    std::sprintf(percentage, "%d.%02d", chargePercentage / 100, chargePercentage % 100);
    lcd.setTextForSection(6, percentage);

    char bat2MinTemp[32];
    std::sprintf(bat2MinTemp, "%d C", batteryTwoMinTemp);
    lcd.setTextForSection(7, bat2MinTemp);

    char bat2MaxTemp[32];
    std::sprintf(bat2MaxTemp, "%d C", batteryTwoMaxTemp);
    lcd.setTextForSection(8, bat2MaxTemp);
}

void LCDDisplay::setStatus(const char *str) {
    status = str;
}
