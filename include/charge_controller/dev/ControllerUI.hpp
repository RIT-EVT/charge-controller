#ifndef CONTROLLERUI_HPP
#define CONTROLLERUI_HPP

#include <charge_controller/dev/LCDDisplay.hpp>
#include <EVT/dev/Button.hpp>
#include <EVT/dev/Encoder.hpp>
#include <charge_controller/dev/Debounce.hpp>

#define MAXVOLTAGE 400 //TODO these are placeholder values, talk to the EEs about the actual values
#define MAXCURRENT 6
#define DEBOUNCE_TIME 500 //TODO check the encoder documentation for the debounce time

namespace DEV = EVT::core::DEV;

class ControllerUI {
public:
    /**
     * enum that represents what set of selectable items the controller is focused on.
     * The value of each entry is equal to the range of the encoder
     */
    enum State {
        PAGESELECT = 1,
        SETTINGSELECT = 3,
        VOLTAGESELECT = MAXVOLTAGE,
        CURRENTSELECT = MAXCURRENT,
    };

    ControllerUI(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDDisplay& display);

    void process();



private:
    //VARIABLES
    //Object References
    DEV::Encoder& encoder;
    DEV::Button& encoderButton;
    LCDDisplay& display;

    //Internal variables
    State currentState;
    uint16_t savedVoltage;
    uint16_t savedCurrent;

    uint16_t unsavedVoltage;
    uint16_t unsavedCurrent;

    //METHODS

    void setState(State newState);

    enum Settings {
        VOLTAGE = 0,
        CURRENT = 1,
        SAVE = 3,
        QUIT = 4
    };
};

#endif//CONTROLLERUI_HPP
