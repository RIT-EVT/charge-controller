
#include <charge_controller/dev/BMSManager.hpp>

bool BMSManager::isConnected() { return bmsOK2.readPin() == IO::GPIO::State::HIGH; }

bool BMSManager::faultDetected() { return bmsOK1.readPin() == IO::GPIO::State::LOW; }

BMSManager::BMSManager(IO::CAN& can, IO::GPIO& bmsOK1, IO::GPIO& bmsOK2) : bmsOK1(bmsOK1), bmsOK2(bmsOK2){
    this->can = &can;
};

int BMSManager::getStateOfCharge() { return 0; }

void BMSManager::updateBMSData(IO::CANMessage& message) {
    uint8_t length = message.getDataLength();
    LOG.log(Logger::DEBUG, "Got can message with id 0x%X and length %u", message.getId(),message.getDataLength());
    if(message.getId() == TOTAL_VOLTAGE_ID){

        if(length == 4){
            millivolts = *((uint32_t*) message.getPayload());
            LOG.log(Logger::DEBUG,"Got voltage update %u", millivolts);
        }
    } else if(message.getId() == STATE_ID){
        if(length == 1){
            BMSState data = *((BMSState*) message.getPayload());
            LOG.log(Logger::DEBUG,"Got state update %u", data);
            BMSState state = data;
        }
    }
}
