#include <charge_controller/dev/ControllerUI.hpp>
ControllerUI::ControllerUI(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDDisplay& display, ControllerModel& model) : encoder(encoder), encoderButton(encoderButton), display(display), model(model) {
    encoder.setRangeAndPosition(ControllerModel::State::PAGESELECT, 0);
}

void ControllerUI::process() {
    switch(model.getState()) {
    case ControllerModel::State::PAGESELECT:
        model.setPage(static_cast<ControllerModel::Page>(encoder.getPosition()));
        if (encoderButton.debounce(DEBOUNCE_TIME) && model.getPage() == ControllerModel::Page::SETTINGSCREEN) {
            setModelState(ControllerModel::State::SETTINGSELECT);
        }
        break;
    case ControllerModel::State::SETTINGSELECT:
        model.setSetting(static_cast<ControllerModel::Setting>(encoder.getPosition()));
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            switch(encoder.getPosition()) {
            case ControllerModel::Setting::VOLTAGE:
                setModelState(ControllerModel::State::VOLTAGESELECT);
                break;
            case ControllerModel::Setting::CURRENT:
                setModelState(ControllerModel::State::CURRENTSELECT);
                break;
            case ControllerModel::Setting::SAVE:
                model.saveVoltageAndCurrent();
                break;
            case ControllerModel::Setting::QUIT:
                setModelState(ControllerModel::State::PAGESELECT);
                break;
            }
        }
        break;
    case ControllerModel::State::VOLTAGESELECT:
        model.setUnsavedVoltage(encoder.getPosition());
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            setModelState(ControllerModel::State::SETTINGSELECT);
        }
        break;
    case ControllerModel::State::CURRENTSELECT:
        model.setUnsavedVoltage(encoder.getPosition());
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            setModelState(ControllerModel::State::SETTINGSELECT);
        }
        break;
    }
    display.display();
}

void ControllerUI::setModelState(ControllerModel::State newState) {
    if (model.getState() != newState) {
        if (newState == ControllerModel::State::SETTINGSELECT && model.getState() != ControllerModel::State::PAGESELECT) {
            encoder.setRangeAndPosition(newState, encoder.getPosition());
        } else if (newState == ControllerModel::State::VOLTAGESELECT) {
            encoder.setRangeAndPosition(newState, model.getUnsavedVoltage());
        } else if (newState == ControllerModel::State::CURRENTSELECT) {
            encoder.setRangeAndPosition(newState, model.getUnsavedCurrent());
        } else {
            encoder.setRangeAndPosition(newState, 0);
        }
        model.setState(newState);
    }
}