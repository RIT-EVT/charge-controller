#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <EVT/io/SPI.hpp>
#include <cstdint>

namespace IO = EVT::core::IO;

class LCDDisplay {
public:
    LCDDisplay(IO::SPI &spi, IO::GPIO &reset, IO::GPIO &A0);
    void init();
    void setVoltage(float volts);
    void setTemperature(float temperature);
    void setCharge(float chargePercentage);
    void setStatus(const char *str, uint16_t length);
    void setMessage(const char *str, uint16_t length);

    void showSplashScreen();
    void show();

private:
};

#endif // LCDDISPLAY_H
