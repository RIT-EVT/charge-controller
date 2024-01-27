#include <charge_controller/dev/ControllerUI.hpp>

ControllerUI::ControllerUI(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDDisplay& display, ControllerModel& model) : model(model), encoder(encoder), encoderButton(encoderButton), display(display) {
    encoder.setRangeAndPosition(model.getEncoderRange(ControllerModel::State::PAGE_SELECT), 0);
}

void ControllerUI::process() {
    EVT::core::log::LOGGER.log(EVT::core::log::Logger::LogLevel::DEBUG, "ENCODER: %ld\n\r", encoder.getPosition());
    switch(model.getState()) {
    case ControllerModel::State::PAGE_SELECT:
        switch(encoder.getPosition()) {
            case ControllerModel::Page::MAIN:
                model.setPage(ControllerModel::Page::MAIN);
                break;
            case ControllerModel::Page::SETTINGS:
                model.setPage(ControllerModel::Page::SETTINGS);
                break;
            default:
                //THIS SHOULD NEVER HAPPEN
                break;
        }
        if (encoderButton.debounce(DEBOUNCE_TIME) && model.getPage() == ControllerModel::Page::SETTINGS) {
            setModelState(ControllerModel::State::SETTING_SELECT);
        }
        break;
    case ControllerModel::State::SETTING_SELECT:
        switch(encoder.getPosition()) {
            case ControllerModel::SelectedSetting::VOLTAGE:
                model.setSelectedSetting(ControllerModel::SelectedSetting::VOLTAGE);
                break;
            case ControllerModel::SelectedSetting::CURRENT:
                model.setSelectedSetting(ControllerModel::SelectedSetting::CURRENT);
                break;
            case ControllerModel::SelectedSetting::SAVE:
                model.setSelectedSetting(ControllerModel::SelectedSetting::SAVE);
                break;
            case ControllerModel::SelectedSetting::QUIT:
                model.setSelectedSetting(ControllerModel::SelectedSetting::QUIT);
                break;
            default:
                //THIS SHOULD NEVER HAPPEN!!!!
                break;
        }
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
    EVT::core::log::LOGGER.log(EVT::core::log::Logger::LogLevel::DEBUG, "Setting State to: %d\n", newState);
    if (model.getState() != newState) {
        switch(newState) {
            case ControllerModel::State::SETTING_SELECT:
                if (model.getState() != ControllerModel::State::PAGE_SELECT) {
                    encoder.setRangeAndPosition(model.getEncoderRange(newState), model.getSelectedSetting());
                } else {
                    encoder.setRangeAndPosition(model.getEncoderRange(newState), 0);
                }
                break;
            case ControllerModel::State::VOLTAGE_SELECT:
                encoder.setRangeAndPosition(model.getEncoderRange(newState), model.getUnsavedVoltage());
                break;
            case ControllerModel::State::CURRENT_SELECT:
                encoder.setRangeAndPosition(model.getEncoderRange(newState), model.getUnsavedCurrent());
                break;
            default:
                encoder.setRangeAndPosition(model.getEncoderRange(newState), 0);
                break;
        }
        model.setState(newState);
    }
}