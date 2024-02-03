#ifndef CHARGE_CONTROLLER_CONTROLLERMODEL_HPP
#define CHARGE_CONTROLLER_CONTROLLERMODEL_HPP

#include <cstdint>

#define MAX_VOLTAGE 100
#define MAX_CURRENT 60

#define DEFAULT_VOLTAGE 96
#define DEFAULT_CURRENT 60

/**
 * Stores values that both the LCDDisplay and the ControllerUI need to access.
 * Acts as the model in a Model-View-Controller (MVC) Structure where
 * ControllerModel is the model,
 * LCDDisplay is the display, and
 * ControllerUI is the controller
 */

class ControllerModel {

public:
    /**
     * default (and only) initializer for the ControllerModel class.
     */
    ControllerModel();

    /**
     * enum for the different pages that the LCDDisplay can display.
     */
    enum Page {
        MAIN = 0,
        SETTINGS = 1
    };

    /**
     * enum that represents what set of selectable items the controller is focused on.
     * The value of each entry is equal to the range of the encoder
     */
    enum State {
        PAGE_SELECT = 0u,
        SETTING_SELECT = 1u,
        VOLTAGE_SELECT = 2u,
        CURRENT_SELECT = 3u,
    };

    /**
     * enum that represents which setting is selected
     * Only updated when state = SETTINGSELECT
     */
    enum SelectedSetting {
        VOLTAGE = 0u,
        CURRENT = 1u,
        SAVE = 2u,
        QUIT = 3u,
        RESET = 4u
    };

    /**
     * Gets the current Page that the model is in.
     *
     * @return the current page the model is set to
     */
    Page getPage();

    /**
     * Gets the current State that the model is in.
     *
     * @return the current state the model is set to
     */
    State getState();

    /**
     * Sets the state of the model
     *
     * @param[in] newState the state to set the model's state to.
     */
    void setState(State newState);

    /**
     * Saves the unsavedVoltage and unsavedCurrent.
     * (Sets savedVoltage and savedCurrent)
     */
    void saveVoltageAndCurrent();

    /**
     * Gets the current setting the model is in: this will be the most recent setting that was selected.
     *
     * @return the current setting the model is in.
     */
    SelectedSetting getSelectedSetting();

    /**
     * Sets the selected setting of the model.
     *
     * @param[in] newSetting the new setting that was selected.
     */
    void setSelectedSetting(SelectedSetting newSetting);

    /**
     * Sets the unsavedVoltage
     *
     * @param[in] newVoltage the new value for unsavedVoltage
     */
    void setUnsavedVoltage(uint16_t newVoltage);

    /**
     * Sets the unsavedCurrent
     *
     * @param[in] newCurrent the new value for unsavedCurrent
     */
    void setUnsavedCurrent(uint16_t newCurrent);

    /**
     * Sets the model's current page.
     *
     * @param[in] newPage the new page.
     */
    void setPage(Page newPage);

    /**
     * Gets the saved voltage value for the model.
     * @return the saved voltage in the model.
     */
    uint16_t getSavedVoltage();

    /**
     * Gets the saved current value for the model.
     * @return the saved current in the model.
     */
    uint16_t getSavedCurrent();

    /**
     * Gets the unsaved voltage value for the model.
     * @return the unsaved voltage in the model.
     */
    uint16_t getUnsavedVoltage();

    /**
     * Gets the unsaved current value for the model.
     * @return the unsaved current in the model.
     */
    uint16_t getUnsavedCurrent();

    /**
     * Returns the valid encoder range for the given state
     * @param state the state that the encoder range can take
     * @return the encoder range for that state
     */
    uint32_t getEncoderRange(State state);

private:
    /** The current state the model is in */
    State state = PAGE_SELECT;
    /** The current page the lcd is displaying */
    Page page = MAIN;
    /** The most recent selected setting that is selected (only really relevant if state = SETTINGSELECT */
    SelectedSetting setting = QUIT;

    uint16_t savedVoltage = DEFAULT_VOLTAGE;//TODO see about saving values between boot sequences
    uint16_t savedCurrent = DEFAULT_CURRENT;

    uint16_t unsavedVoltage = DEFAULT_VOLTAGE;
    uint16_t unsavedCurrent = DEFAULT_CURRENT;
};

#endif//CHARGE_CONTROLLER_CONTROLLERMODEL_HPP
