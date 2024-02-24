#include <charge_controller/dev/UIModel.hpp>

CC::UIModel::UIModel() = default;

CC::UIModel::Page CC::UIModel::getPage() {
    return page;
}

CC::UIModel::State CC::UIModel::getState() {
    return state;
}

void CC::UIModel::setState(CC::UIModel::State newState) {
    state = newState;
}

void CC::UIModel::saveVoltageAndCurrent() {
    savedVoltage = unsavedVoltage;
    savedCurrent = unsavedCurrent;
}

void CC::UIModel::setUnsavedVoltage(uint16_t newVoltage) {
    unsavedVoltage = newVoltage;
}

void CC::UIModel::setUnsavedCurrent(uint16_t newCurrent) {
    unsavedCurrent = newCurrent;
}

void CC::UIModel::setPage(CC::UIModel::Page newPage) {
    page = newPage;
}

uint16_t CC::UIModel::getSavedVoltage() {
    return savedVoltage;
}

uint16_t CC::UIModel::getSavedCurrent() {
    return savedCurrent;
}

uint16_t CC::UIModel::getUnsavedVoltage() {
    return unsavedVoltage;
}

uint16_t CC::UIModel::getUnsavedCurrent() {
    return unsavedCurrent;
}

uint32_t CC::UIModel::getEncoderRange(State state) {
    switch (state) {
    case PAGE_SELECT: return 1;
    case SETTING_SELECT: return 4;
    case VOLTAGE_SELECT: return MAX_VOLTAGE;
    case CURRENT_SELECT: return MAX_CURRENT;
    }
}

CC::UIModel::SelectedSetting CC::UIModel::getSelectedSetting() {
    return setting;
}

void CC::UIModel::setSelectedSetting(SelectedSetting newSetting) {
    setting = newSetting;
}
