#ifndef CONTROLLERUI_HPP
#define CONTROLLERUI_HPP

#include <EVT/dev/Encoder.hpp>
#include <EVT/dev/Button.hpp>
#include <charge_controller/dev/Debounce.hpp>

#define MAXVOLTAGE 400 //TODO these are placeholder values, talk to the EEs about the actual values
#define MAXCURRENT 6

namespace DEV = EVT::core::DEV;

class ControllerUI {
public:
    /**
     * enum that represents what set of selectable items the controller is focused on
     * the number is equal to the range of the encoder
     */
    enum state {
        PAGESELECT = 2,
        SETTINGSELECT = 4,
        VOLTAGESELECT = MAXVOLTAGE,
        CURRENTSELECT = MAXCURRENT,
    };

    ControllerUI(Encoder& encoder, Button& encoderButton, LCDDisplay& display);
private:

};

#endif//CONTROLLERUI_HPP
