#include <charge_controller/dev/ControllerUI.hpp>
ControllerUI::ControllerUI(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDDisplay& display) : encoder(encoder), encoderButton(encoderButton), display(display) {}

void ControllerUI::process() {
    switch(currentState) {
    case PAGESELECT:
        display.display(static_cast<LCDDisplay::Page>(encoder.getPosition()));
        break;
    case SETTINGSELECT:
        display.display(LCDDisplay::Page::SETTINGSCREEN);
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            switch(encoder.getPosition()) {
            case Settings::VOLTAGE:
                currentState = VOLTAGESELECT;
                break;
            case Settings::CURRENT:
                currentState = CURRENTSELECT;
                break;
            case Settings::SAVE:
                savedCurrent = unsavedCurrent;
                savedVoltage = unsavedVoltage;
                break;
            case Settings::QUIT:
                currentState = PAGESELECT;
                break;
            }
        }
        break;
    case VOLTAGESELECT:
        display.display(LCDDisplay::Page::SETTINGSCREEN);
        unsavedVoltage = encoder.getPosition();
        break;
    case CURRENTSELECT:
        display.display(LCDDisplay::Page::SETTINGSCREEN);
        unsavedCurrent = encoder.getPosition();
        break;
    }
}

void ControllerUI::setState(State newState) {
    if (currentState != newState) {
        currentState = newState;
        encoder.setRangeAndPosition(currentState, 0);
    }
}