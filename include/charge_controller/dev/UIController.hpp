#ifndef CONTROLLERUI_HPP
#define CONTROLLERUI_HPP

#include <EVT/dev/Encoder.hpp>
#include <EVT/dev/button.hpp>
#include <EVT/utils/log.hpp>
#include <charge_controller/dev/LCDView.hpp>
#include <charge_controller/dev/UIModel.hpp>

#define DEBOUNCE_TIME 500

namespace DEV = EVT::core::DEV;

namespace CC {

/**
 * Outputs all the values from the ChargeController class to the LCD display
 * Acts as the view in a Model-View-Controller (MVC) design pattern where
 * UIModel is the model,
 * LCDView is the view, and
 * UIController is the controller.
 */
class UIController {
public:
    /**
     * Constructor for the ControllerUI class
     *
     * @param[in] encoder The encoder object
     * @param[in] encoderButton the button on the encoder
     * @param[in] display the LCDDisplay that the ControllerUI controls
     * @param[in] model the ControllerModel that stores values from the ControllerUI
     */
    UIController(EVT::core::DEV::Encoder& encoder, EVT::core::DEV::Button& encoderButton, LCDView& display, UIModel& model);

    /**
     * Processes user input, updates appropriate values, and calls display.display() to update the view.
     */
    void process();

private:
    /** Reference to the model object */
    UIModel& model;
    /** Reference to the encoder */
    EVT::core::DEV::Encoder& encoder;
    /** Reference to the encoder button */
    EVT::core::DEV::Button& encoderButton;
    /** Reference to the display driver so it can update it */
    LCDView& display;

    /**
     * Internal method that sets the model's state and also updates
     * the encoder range and position to align with the state.
     *
     * @param newState the new state to set the model's state to.
     */
    void setModelState(UIModel::State newState);

    /**
     * Internal helper method for process.
     * Handles logic when the UIModel is in PAGESELECT mode.
     *
     * @param[in] page which page the user is selecting.
     */
    void switchSelectedPage(UIModel::Page page);

    /**
     * Internal helper method for process.
     * Handles logic when the UIModel is in SETTINGSELECT mode.
     *
     * @param[in] setting which setting the user is selecting.
     */
    void switchSelectedSetting(UIModel::SelectedSetting setting);

    /**
     * Internal helper method for process.
     * Handles logic when the UIModel is in SETTINGSELECT mode
      * and the encoder button has been pressed.
     *
     * @param[in] setting which setting the user has selected.
     */
    void handleButtonPressSettingSelect(UIModel::SelectedSetting setting);
};

}//namespace CC

#endif//CONTROLLERUI_HPP
