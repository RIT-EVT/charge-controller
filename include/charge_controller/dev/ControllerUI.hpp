#ifndef CONTROLLERUI_HPP
#define CONTROLLERUI_HPP

#include <EVT/dev/Encoder.hpp>
#include <EVT/dev/button.hpp>
#include <EVT/utils/log.hpp>
#include <charge_controller/dev/ControllerModel.hpp>
#include <charge_controller/dev/EncoderButton.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>

//TODO check the encoder documentation for the debounce time
#define DEBOUNCE_TIME 500

namespace DEV = EVT::core::DEV;

/**
 * Handles the encoder inputs for the Charge Controller.
 * Acts as the controller in a Model-View-Controller (MVC) Structure where
 * ControllerModel is the model,
 * LCDDisplay is the display, and
 * ControllerUI is the controller
 */

class ControllerUI {
public:

    /**
     * Constructor for the ControllerUI class
     *
     * @param[in] encoder The encoder object
     * @param[in] encoderButton the button on the encoder
     * @param[in] display the LCDDisplay that the ControllerUI controls
     * @param[in] model the ControllerModel that stores values from the ControllerUI
     */
    ControllerUI(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDDisplay& display, ControllerModel& model);

    /**
     * Processes user input, updates appropriate values, and calls display.display() to update the view
     */
    void process();


private:
    /** Reference to the model object */
    ControllerModel& model;
    /** Reference to the encoder */
    DEV::Encoder& encoder;
    /** Reference to the encoder's button */
    DEV::Button& encoderButton;
    /** Reference to the display driver so it can update */
    LCDDisplay& display;

    /**
     * Internal method that sets the model's state and also updates the encoder range and position to align with the state.
     * @param newState the new state to set the model's state to.
     */
    void setModelState(ControllerModel::State newState);
};

#endif//CONTROLLERUI_HPP
