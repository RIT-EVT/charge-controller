#include <charge_controller/dev/ControllerUI.hpp>
ControllerUI::ControllerUI(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDDisplay& display, ControllerModel& model) : model(model), encoder(encoder), encoderButton(encoderButton), display(display) {
    encoder.setRangeAndPosition(ControllerModel::State::PAGE_SELECT, 0);
}

void ControllerUI::process() {
    switch(model.getState()) {
    case ControllerModel::State::PAGE_SELECT:
        model.setPage(static_cast<ControllerModel::Page>(encoder.getPosition()));
        if (encoderButton.debounce(DEBOUNCE_TIME) && model.getPage() == ControllerModel::Page::SETTINGS) {
            setModelState(ControllerModel::State::SETTING_SELECT);
        }
        break;
    case ControllerModel::State::SETTING_SELECT:
        model.setSelectedSetting(static_cast<ControllerModel::SelectedSetting>(encoder.getPosition()));
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            switch(encoder.getPosition()) {
            case ControllerModel::SelectedSetting::VOLTAGE:
                setModelState(ControllerModel::State::VOLTAGE_SELECT);
                break;
            case ControllerModel::SelectedSetting::CURRENT:
                setModelState(ControllerModel::State::CURRENT_SELECT);
                break;
            case ControllerModel::SelectedSetting::SAVE:
                model.saveVoltageAndCurrent();
                break;
            case ControllerModel::SelectedSetting::QUIT:
                setModelState(ControllerModel::State::PAGE_SELECT);
                break;
            }
        }
        break;
    case ControllerModel::State::VOLTAGE_SELECT:
        model.setUnsavedVoltage(encoder.getPosition());
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            setModelState(ControllerModel::State::SETTING_SELECT);
        }
        break;
    case ControllerModel::State::CURRENT_SELECT:
        model.setUnsavedCurrent(encoder.getPosition());
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            setModelState(ControllerModel::State::SETTING_SELECT);
        }
        break;
    }
    display.display();
}

void ControllerUI::setModelState(ControllerModel::State newState) {
    if (model.getState() != newState) {
        if (newState == ControllerModel::State::SETTING_SELECT && model.getState() != ControllerModel::State::PAGE_SELECT) {
            encoder.setRangeAndPosition(newState, encoder.getPosition());
        } else if (newState == ControllerModel::State::VOLTAGE_SELECT) {
            encoder.setRangeAndPosition(newState, model.getUnsavedVoltage());
        } else if (newState == ControllerModel::State::CURRENT_SELECT) {
            encoder.setRangeAndPosition(newState, model.getUnsavedCurrent());
        } else {
            encoder.setRangeAndPosition(newState, 0);
        }
        model.setState(newState);
    }
}