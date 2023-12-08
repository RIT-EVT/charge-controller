#ifndef CONTROLLERUI_HPP
#define CONTROLLERUI_HPP

#include <charge_controller/dev/ControllerModel.hpp>
#include <EVT/dev/button.hpp>
#include <EVT/dev/Encoder.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>

#define DEBOUNCE_TIME 500 //TODO check the encoder documentation for the debounce time

namespace DEV = EVT::core::DEV;

class ControllerUI {
public:

    ControllerUI(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDDisplay& display, ControllerModel& model);

    void process();


private:
    //Object References
    ControllerModel& model;
    DEV::Encoder& encoder;
    DEV::Button& encoderButton;
    LCDDisplay& display;

    void setModelState(ControllerModel::State newState);
};

#endif//CONTROLLERUI_HPP
