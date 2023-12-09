#include <charge_controller/dev/ControllerModel.hpp>

ControllerModel::ControllerModel() = default;

ControllerModel::Page ControllerModel::getPage() {
    return page;
}

ControllerModel::State ControllerModel::getState() {
    return state;
}

void ControllerModel::setState(ControllerModel::State newState) {
    state = newState;
}

void ControllerModel::saveVoltageAndCurrent() {
    savedVoltage = unsavedVoltage;
    savedCurrent = unsavedCurrent;
}

void ControllerModel::setUnsavedVoltage(uint16_t newVoltage) {
    unsavedVoltage = newVoltage;
}

void ControllerModel::setUnsavedCurrent(uint16_t newCurrent) {
    unsavedCurrent = newCurrent;
}

void ControllerModel::setPage(ControllerModel::Page newPage) {
    page = newPage;
}

uint16_t ControllerModel::getSavedVoltage() {
    return savedVoltage;
}

uint16_t ControllerModel::getSavedCurrent() {
    return savedCurrent;
}

uint16_t ControllerModel::getUnsavedVoltage() {
    return unsavedVoltage;
}

uint16_t ControllerModel::getUnsavedCurrent() {
    return unsavedCurrent;
}

ControllerModel::SelectedSetting ControllerModel::getSelectedSetting() {
    return setting;
}

void ControllerModel::setSelectedSetting(SelectedSetting newSetting) {
    setting = newSetting;
}
