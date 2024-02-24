#include <charge_controller/dev/UIController.hpp>

namespace log = EVT::core::log;

CC::UIController::UIController(DEV::Encoder& encoder, DEV::Button& encoderButton, LCDView& display, UIModel& model) : model(model), encoder(encoder), encoderButton(encoderButton), display(display) {
    encoder.setRangeAndPosition(model.getEncoderRange(UIModel::State::PAGE_SELECT), 0);
}

void CC::UIController::process() {
    switch (model.getState()) {
    case UIModel::State::PAGE_SELECT:
        switchSelectedPage((UIModel::Page) encoder.getPosition());
        break;
    case UIModel::State::SETTING_SELECT:
        switchSelectedSetting((UIModel::SelectedSetting) encoder.getPosition());
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            handleButtonPressSettingSelect((UIModel::SelectedSetting) encoder.getPosition());
        }
        break;
    case UIModel::State::VOLTAGE_SELECT:
        model.setUnsavedVoltage(encoder.getPosition());
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            setModelState(UIModel::State::SETTING_SELECT);
        }
        break;
    case UIModel::State::CURRENT_SELECT:
        model.setUnsavedCurrent(encoder.getPosition());
        if (encoderButton.debounce(DEBOUNCE_TIME)) {
            setModelState(UIModel::State::SETTING_SELECT);
        }
        break;
    }

    //display all the changes.
    display.display();
}

void CC::UIController::setModelState(UIModel::State newState) {
    UIModel::State currentState = model.getState();
    if (currentState != newState) {
        switch (newState) {
        case UIModel::State::SETTING_SELECT:
            if (currentState != UIModel::State::PAGE_SELECT) {
                encoder.setRangeAndPosition(model.getEncoderRange(newState), model.getSelectedSetting());
            } else {
                encoder.setRangeAndPosition(model.getEncoderRange(newState), 0);
            }
            break;
        case UIModel::State::VOLTAGE_SELECT:
            encoder.setRangeAndPosition(model.getEncoderRange(newState), model.getUnsavedVoltage());
            break;
        case UIModel::State::CURRENT_SELECT:
            encoder.setRangeAndPosition(model.getEncoderRange(newState), model.getUnsavedCurrent());
            break;
        default:
            encoder.setRangeAndPosition(model.getEncoderRange(newState), 0);
            break;
        }
        model.setState(newState);
    }
}

void CC::UIController::switchSelectedPage(UIModel::Page page) {
    switch (page) {
    case UIModel::Page::MAIN:
        model.setPage(UIModel::Page::MAIN);
        break;
    case UIModel::Page::SETTINGS:
        model.setPage(UIModel::Page::SETTINGS);
        break;
    default:
        log::LOGGER.log(log::Logger::LogLevel::ERROR, "Encoder position out of range of Controller::Model::Page options");
        break;
    }
    if (encoderButton.debounce(DEBOUNCE_TIME) && model.getPage() == UIModel::Page::SETTINGS) {
        setModelState(UIModel::State::SETTING_SELECT);
    }
}

void CC::UIController::switchSelectedSetting(UIModel::SelectedSetting setting) {
    switch (setting) {
    case UIModel::SelectedSetting::VOLTAGE:
        model.setSelectedSetting(UIModel::SelectedSetting::VOLTAGE);
        break;
    case UIModel::SelectedSetting::CURRENT:
        model.setSelectedSetting(UIModel::SelectedSetting::CURRENT);
        break;
    case UIModel::SelectedSetting::SAVE:
        model.setSelectedSetting(UIModel::SelectedSetting::SAVE);
        break;
    case UIModel::SelectedSetting::QUIT:
        model.setSelectedSetting(UIModel::SelectedSetting::QUIT);
        break;
    case UIModel::SelectedSetting::RESET:
        model.setSelectedSetting(UIModel::SelectedSetting::RESET);
    default:
        log::LOGGER.log(log::Logger::LogLevel::ERROR, "Encoder position out of range of Controller::Model::SelectedSetting options");
        break;
    }
}

void CC::UIController::handleButtonPressSettingSelect(UIModel::SelectedSetting setting) {
    switch (setting) {
    case UIModel::SelectedSetting::VOLTAGE:
        setModelState(UIModel::State::VOLTAGE_SELECT);
        break;
    case UIModel::SelectedSetting::CURRENT:
        setModelState(UIModel::State::CURRENT_SELECT);
        break;
    case UIModel::SelectedSetting::SAVE:
        model.saveVoltageAndCurrent();
        break;
    case UIModel::SelectedSetting::QUIT:
        setModelState(UIModel::State::PAGE_SELECT);
        model.setUnsavedVoltage(model.getSavedVoltage());
        model.setUnsavedCurrent(model.getSavedCurrent());
        break;
    case UIModel::SelectedSetting::RESET:
        model.setUnsavedVoltage(DEFAULT_VOLTAGE);
        model.setUnsavedCurrent(DEFAULT_CURRENT);
        break;
    default:
        log::LOGGER.log(log::Logger::LogLevel::ERROR, "Encoder position out of range of Controller::Model::SelectedSetting options");
        break;
    }
}
