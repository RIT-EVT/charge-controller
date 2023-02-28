
#include <charge_controller/dev/BMSManager.hpp>



BMSManager::BMSManager(IO::CAN& can, IO::GPIO *bmsOK[]) {
    this->can = &can;
    for (int i = 0; i < MAX_BMS_PACKS; i++) {
        this->bmsOK[i] = bmsOK[i];
    }
};

//int BMSManager::getStateOfCharge() { return 0; }
//
//void BMSManager::updateBMSData(IO::CANMessage& message) {
//    uint8_t length = message.getDataLength();
//    LOG.log(Logger::DEBUG, "Got can message with id 0x%X and length %u", message.getId(),message.getDataLength());
//    if(message.getId() == TOTAL_VOLTAGE_ID){
//
//        if(length == 4){
//            millivolts = *((uint32_t*) message.getPayload());
//            LOG.log(Logger::DEBUG,"Got voltage update %u", millivolts);
//        }
//    } else if(message.getId() == STATE_ID){
//        if(length == 1){
//            BMSStatus data = *((BMSStatus*) message.getPayload());
//            LOG.log(Logger::DEBUG,"Got state update %u", data);
//            BMSStatus state = data;
//        }
//    }
//}

uint8_t BMSManager::numConnected() {
    uint8_t count = 0;
    for(int i = 0; i< MAX_BMS_PACKS; i++){
        if(packs[i].isConnected){
            count++;
        }
    }
    return count;
}

bool BMSManager::isConnected(uint8_t packNum) {
    return packs[packNum].isConnected;
}

bool BMSManager::faultDetected(uint8_t packNum) {
    return bmsOK[packNum]->readPin() == BMS_NOT_OK ||
           packs[packNum].data.status == BMSStatus::UNSAFE_CONDITIONS_ERROR ||
           packs[packNum].data.status == BMSStatus::INITIALIZATION_ERROR;
}

bool BMSManager::isCharging(uint8_t packNum) {
    return packs[packNum].data.status == BMSStatus::CHARGING;
}

bool BMSManager::isReady(uint8_t packNum) {
    return  packs[packNum].data.status == BMSStatus::SYSTEM_READY ||
            packs[packNum].data.status == BMSStatus::DEEP_SLEEP ||
            packs[packNum].data.status == BMSStatus::CHARGING;
}

int16_t BMSManager::getBatteryVoltage(uint8_t packNum) {
    return packs[packNum].data.batteryVoltage;
}

int16_t BMSManager::getMinCellVoltage(uint8_t packNum) {
    return packs[packNum].data.minCellVoltage;
}

uint8_t BMSManager::getMinCellVoltageID(uint8_t packNum) {
    return packs[packNum].data.minCellVoltageID;
}

int16_t BMSManager::getMaxCellVoltage(uint8_t packNum) {
    return packs[packNum].data.maxCellVoltage;
}

uint8_t BMSManager::getMaxCellVoltageID(uint8_t packNum) {
    return packs[packNum].data.maxCellVoltageID;
}

int8_t BMSManager::getBatteryMinTemp(uint8_t packNum) {
    return packs[packNum].data.batteryPackMinTemp;
}

int8_t BMSManager::getBatteryMaxTemp(uint8_t packNum) {
    return packs[packNum].data.batteryPackMaxTemp;
}

BMSManager::BMSStatus BMSManager::getStatus(uint8_t packNum) {
    return packs[packNum].data.status;
}

uint8_t BMSManager::getNumElements() {
    return OBJECT_DICTIONARY_SIZE;
}

CO_OBJ_T *BMSManager::getObjectDictionary() {
    return &objectDictionaryBMS[0];
}


void BMSManager::printDebug() {
    for(int i = 0; i<MAX_BMS_PACKS; i++) {
        if (packs[i].data.batteryVoltage != lastValues[i].batteryVoltage) {
            LOG.log(Logger::DEBUG, "Pack%d->batteryVoltage=%d",i, packs[i].data.batteryVoltage);
        }
        if (packs[i].data.minCellVoltage != lastValues[i].minCellVoltage) {
            LOG.log(Logger::DEBUG, "Pack%d->minCellVoltage=%d",i, packs[i].data.minCellVoltage);
        }
        if (packs[i].data.minCellVoltageID != lastValues[i].minCellVoltageID) {
            LOG.log(Logger::DEBUG, "Pack%d->minCellVoltageID=%d",i, packs[i].data.minCellVoltageID);
        }
        if (packs[i].data.maxCellVoltage != lastValues[i].maxCellVoltage) {
            LOG.log(Logger::DEBUG, "Pack%d->maxCellVoltage=%d",i, packs[i].data.maxCellVoltage);
        }
        if (packs[i].data.maxCellVoltageID != lastValues[i].maxCellVoltageID) {
            LOG.log(Logger::DEBUG, "Pack%d->maxCellVoltageID=%d",i, packs[i].data.maxCellVoltageID);
        }
        if (packs[i].data.batteryPackMinTemp != lastValues[i].batteryPackMinTemp) {
            LOG.log(Logger::DEBUG, "Pack%d->batteryPackMinTemp=%d",i, packs[i].data.batteryPackMinTemp);
        }
        if (packs[i].data.batteryPackMaxTemp != lastValues[i].batteryPackMaxTemp) {
            LOG.log(Logger::DEBUG, "Pack%d->batteryPackMaxTemp=%d",i, packs[i].data.batteryPackMaxTemp);
        }
        if (packs[i].data.status != lastValues[i].status) {
            LOG.log(Logger::DEBUG, "Pack%d->status=%d",i, packs[i].data.status);
        }
        lastValues[i] = packs[i].data;
    }
}

void BMSManager::update() {
    for(int i = 0; i< MAX_BMS_PACKS; i++){
        packs[i].isConnected = bmsOK[i]->readPin() == IO::GPIO::State::HIGH;
    }
}