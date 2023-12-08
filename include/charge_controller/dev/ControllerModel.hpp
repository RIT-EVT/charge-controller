#ifndef CHARGE_CONTROLLER_CONTROLLERMODEL_HPP
#define CHARGE_CONTROLLER_CONTROLLERMODEL_HPP

#include <cstdint>


#define MAXVOLTAGE 400 //TODO these are placeholder values, talk to the EEs about the actual values
#define MAXCURRENT 6
#define DEFAULTVOLTAGE 96
#define DEFAULTCURRENT 60

class ControllerModel {

public:

    ControllerModel();

    /**
     * enum for the different pages that the LCDDisplay can display.
     */
    enum Page {
        MAINSCREEN = 0,
        SETTINGSCREEN = 1
    };

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

    /**
     * enum that represents which setting if state = SETTINGSELECT is currently selected.
     */
    enum Setting {
        VOLTAGE = 0,
        CURRENT = 1,
        SAVE = 3,
        QUIT = 4
    };

    Page getPage();

    State getState();

    void setState(State newState);

    void saveVoltageAndCurrent();

    Setting getSetting();

    void setSetting(Setting newSetting);

    void setUnsavedVoltage(uint16_t newVoltage);

    void setUnsavedCurrent(uint16_t newCurrent);

    void setPage(Page newPage);


    uint16_t getSavedVoltage();

    uint16_t getSavedCurrent();

    uint16_t getUnsavedVoltage();

    uint16_t getUnsavedCurrent();

private:

    State state = PAGESELECT;
    Page page = MAINSCREEN;
    Setting setting = QUIT;

    //Internal variables
    uint16_t savedVoltage = DEFAULTVOLTAGE; //TODO see about saving values between boot sequences
    uint16_t savedCurrent = DEFAULTCURRENT;

    uint16_t unsavedVoltage = DEFAULTVOLTAGE;
    uint16_t unsavedCurrent = DEFAULTCURRENT;

};

#endif//CHARGE_CONTROLLER_CONTROLLERMODEL_HPP
