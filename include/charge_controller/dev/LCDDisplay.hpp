#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

class LCDDisplay{
public:
    LCDDisplay(IO::SPI& spi, IO::GPIO& reset, IO::GPIO& A0);
    void init();
    void setVoltage(float volts);
    void setTemperature(float temperature);
    void setCharge(float chargePercentage);
    void setStatus(const char* str, size_t length);
    void setMessage(const char* str, size_t length);

    void showSplashScreen();
    void show();
private:

};

#endif //LCDDISPLAY_H
