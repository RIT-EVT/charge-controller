// clang-format off
#include <EVT/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/utils/log.hpp>
#include <EVT/utils/time.hpp>
#include <EVT/utils/types/FixedQueue.hpp>
#include <EVT/dev/platform/f3xx/Timerf3xx.hpp> // TODO: clang will put this at the top causing compilation problem
// clang-format on

#include <EVT/io/CANopen.hpp>
#include <Canopen/co_core.h>
#include <Canopen/co_tmr.h>

#include <charge_controller/ChargeController.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <charge_controller/dev/Debounce.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>
#include "EVT/dev/button.hpp"

namespace IO = EVT::core::IO;
namespace DEV = EVT::core::DEV;
namespace time = EVT::core::time;
namespace log = EVT::core::log;
namespace platform = EVT::core::platform;

#define HEARTBEAT_INTERVAL 1000

// IO pins
constexpr IO::Pin STANDBY_BUTTON_PIN = IO::Pin::PA_4;
constexpr IO::Pin START_BUTTON_PIN = IO::Pin::PA_6;
constexpr IO::Pin RESET_BUTTON_PIN = IO::Pin::PA_1;

constexpr IO::Pin ENCODER_BUTTON_PIN = IO::Pin::PB_4;
constexpr IO::Pin ENCODER_A = IO::Pin::PA_8;
constexpr IO::Pin ENCODER_B = IO::Pin::PA_9;

constexpr IO::Pin BATTERY_1_OK = IO::Pin::PB_6;
constexpr IO::Pin BATTERY_2_OK = IO::Pin::PB_5;

constexpr IO::Pin LED_PIN = IO::Pin::PA_10;

constexpr uint32_t SPI_SPEED = SPI_SPEED_500KHZ;

struct CANInterruptParams {
    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage>* queue;
    ChargeController* chargeController;
};

/**
 * Interrupt handler to get CAN messages. A function pointer to this function
 * will be passed to the EVT-core CAN interface which will in turn call this
 * function each time a new CAN message comes in.
 *
 * NOTE: For this sample, every non-extended (so 11 bit CAN IDs) will be
 * assumed to be intended to be passed as a CANopen message.
 *
 * @param message[in] The passed in CAN message that was read.
 */
void canInterrupt(IO::CANMessage& message, void* priv) {
    struct CANInterruptParams* params = (CANInterruptParams*) priv;

    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage>* queue = params->queue;
//    log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Received Message 0x%x", message.getId());


    if (message.getId() == ChargeController::CHARGER_STATUS_CAN_ID) {
        // Display the received current and voltage from the charger.
        uint16_t voltage = ((uint16_t) message.getPayload()[0] << 8) | message.getPayload()[1];
        uint16_t current = ((uint16_t) message.getPayload()[2] << 8) | message.getPayload()[3];

        params->chargeController->setDisplayChargerValues(voltage, current);
    }

    if (queue != nullptr)
        queue->append(message);
}


///////////////////////////////////////////////////////////////////////////////
// EVT-core CAN callback and CAN setup. This will include logic to set
// aside CANopen messages into a specific queue
///////////////////////////////////////////////////////////////////////////////

/**
 * Interrupt handler to get CAN messages. A function pointer to this function
 * will be passed to the EVT-core CAN interface which will in turn call this
 * function each time a new CAN message comes in.
 *
 * NOTE: For this sample, every non-extended (so 11 bit CAN IDs) will be
 * assumed to be intended to be passed as a CANopen message.
 *
 * @param message[in] The passed in CAN message that was read.
 */

///////////////////////////////////////////////////////////////////////////////
// CANopen specific Callbacks. Need to be defined in some location
///////////////////////////////////////////////////////////////////////////////
extern "C" void CONodeFatalError(void) {}

extern "C" void COIfCanReceive(CO_IF_FRM* frm) {}

extern "C" int16_t COLssStore(uint32_t baudrate, uint8_t nodeId) { return 0; }

extern "C" int16_t COLssLoad(uint32_t* baudrate, uint8_t* nodeId) { return 0; }

extern "C" void CONmtModeChange(CO_NMT* nmt, CO_MODE mode) {}

extern "C" void CONmtHbConsEvent(CO_NMT* nmt, uint8_t nodeId) {}

extern "C" void CONmtHbConsChange(CO_NMT* nmt, uint8_t nodeId, CO_MODE mode) {}

extern "C" int16_t COParaDefault(CO_PARA* pg) { return 0; }

extern "C" void COPdoTransmit(CO_IF_FRM* frm) {}

extern "C" int16_t COPdoReceive(CO_IF_FRM* frm) { return 0; }

extern "C" void COPdoSyncUpdate(CO_RPDO* pdo) {}

extern "C" void COTmrLock(void) {}

extern "C" void COTmrUnlock(void) {}

extern "C" void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan) {
    log::LOGGER.log(log::Logger::LogLevel::DEBUG, "RX Full");
}

IO::GPIO* devices[1];

int main() {
    platform::init();

    // Pin initialization
    IO::UART& uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(9600);
    uart.printf("Uart Init\n\r");
    log::LOGGER.setUART(&uart);

    // GPIO pins
    IO::GPIO& statusLED = IO::getGPIO<LED_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO* bmsOK[] = {
        &IO::getGPIO<BATTERY_1_OK>(IO::GPIO::Direction::INPUT),
        &IO::getGPIO<BATTERY_2_OK>(IO::GPIO::Direction::INPUT)
    };
    uart.printf("GPIO Init\n\r");

    // Buttons
    IO::GPIO& standbyButtonGPIO =  IO::getGPIO<STANDBY_BUTTON_PIN>(IO::GPIO::Direction::INPUT);
    IO::GPIO& startButtonGPIO =  IO::getGPIO<START_BUTTON_PIN>(IO::GPIO::Direction::INPUT);

//    DEV::Button standbyButton = DEV::Button(standbyButtonGPIO);
    DEV::Button startButton = DEV::Button(startButtonGPIO);

    uart.printf("Buttons Init\n\r");

    // LCD
    IO::GPIO& LCDRegisterSEL = IO::getGPIO<IO::Pin::PB_0>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO& LCDReset = IO::getGPIO<IO::Pin::PA_1>(IO::GPIO::Direction::OUTPUT);

    devices[0] = &IO::getGPIO<IO::Pin::PA_0>(EVT::core::IO::GPIO::Direction::OUTPUT);
    devices[0]->writePin(IO::GPIO::State::HIGH);

    uart.printf("LCD Init\n\r");

    // Spi
    IO::SPI& spi = IO::getSPI<IO::Pin::PA_5, IO::Pin::PA_7>(devices, 1);
    spi.configureSPI(SPI_SPEED, SPI_MODE0, SPI_MSB_FIRST);
    uart.printf("Spi Init\n\r");

    // Initialize CAN, add an IRQ which will add messages to the queue above
    IO::CAN& can = IO::getCAN<IO::Pin::PA_12, IO::Pin::PA_11>();

    // charge controller module instantiation
    BMSManager bms(can, bmsOK);
    LCDDisplay display(LCDRegisterSEL, LCDReset, spi);
    ChargeController chargeController(bms, display, can);

    uart.printf("Modules Initialized\n\r");

    DEV::Timerf3xx timer(TIM2, 100);

    log::LOGGER.setLogLevel(log::Logger::LogLevel::DEBUG);

    display.init();
    chargeController.init();
    uint32_t lastHeartBeat = time::millis();
    uint8_t oldCount = 0;

    uart.printf("Finished Inits!\r\n");

    ///////////////////////////////////////////////////////////////////////////
    // Setup CAN configuration, this handles making drivers, applying settings.
    // And generally creating the CANopen stack node which is the interface
    // between the application (the code we write) and the physical CAN network
    ///////////////////////////////////////////////////////////////////////////
    // Will store CANopen messages that will be populated by the EVT-core CAN interrupt
    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage> canOpenQueue;

    struct CANInterruptParams canParams = {
            .queue = &canOpenQueue,
            .chargeController = &chargeController
    };
    can.addIRQHandler(canInterrupt, reinterpret_cast<void*>(&canParams));

    // Reserved memory for CANopen stack usage
    uint8_t sdoBuffer[1][CO_SDO_BUF_BYTE];
    CO_TMR_MEM appTmrMem[4];

    // Attempt to join the CAN network
    IO::CAN::CANStatus result = can.connect();

    //test that the board is connected to the can network
    if (result != IO::CAN::CANStatus::OK) {
        uart.printf("Failed to connect to CAN network\r\n");
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Setup CAN configuration, this handles making drivers, applying settings.
    // And generally creating the CANopen stack node which is the interface
    // between the application (the code we write) and the physical CAN network
    ///////////////////////////////////////////////////////////////////////////
    // Make drivers
    CO_IF_DRV canStackDriver;

    CO_IF_CAN_DRV canDriver;
    CO_IF_TIMER_DRV timerDriver;
    CO_IF_NVM_DRV nvmDriver;

    IO::getCANopenCANDriver(&can, &canOpenQueue, &canDriver);
    IO::getCANopenTimerDriver(&timer, &timerDriver);
    IO::getCANopenNVMDriver(&nvmDriver);

    canStackDriver.Can = &canDriver;
    canStackDriver.Timer = &timerDriver;
    canStackDriver.Nvm = &nvmDriver;

    //setup CANopen Node
    CO_NODE_SPEC canSpec = {
            .NodeId = BMSManager::NODE_ID,
            .Baudrate = IO::CAN::DEFAULT_BAUD,
            .Dict = bms.getObjectDictionary(),
            .DictLen = bms.getObjectDictionarySize(),
            .EmcyCode = NULL,
            .TmrMem = appTmrMem,
            .TmrNum = 16,
            .TmrFreq = 100,
            .Drv = &canStackDriver,
            .SdoBuf = reinterpret_cast<uint8_t*>(&sdoBuffer[0]),
    };

    CO_NODE canNode;

    CONodeInit(&canNode, &canSpec);
    CONodeStart(&canNode);
    CONmtSetMode(&canNode.Nmt, CO_OPERATIONAL);

    time::wait(500);

    while (true) {
        chargeController.process();
        bms.update();

        if (startButton.debounce(500)) {
            log::LOGGER.log(log::Logger::LogLevel::DEBUG, "Start Pressed");
            if(chargeController.isCharging()) {
                chargeController.stopCharging();
            } else {
                chargeController.startCharging();
            }
        }


        if (bms.numConnected() != oldCount) {
            log::LOGGER.log(log::Logger::LogLevel::DEBUG, "%d batteries connected", bms.numConnected());
            oldCount = bms.numConnected();
        }

        if (time::millis() - lastHeartBeat > HEARTBEAT_INTERVAL) {
            switch (statusLED.readPin()) {
            case IO::GPIO::State::LOW:
                statusLED.writePin(IO::GPIO::State::HIGH);
                break;
            case IO::GPIO::State::HIGH:
                statusLED.writePin(IO::GPIO::State::LOW);
                break;
            }

            // Need to send heartbeat can message to the charger
            chargeController.sendChargerMessage();
            lastHeartBeat = time::millis();
        }

        // Process the BMS RPDOs
        CONodeProcess(&canNode);
        // Update the state of timer based events
        COTmrService(&canNode.Tmr);
        // Handle executing timer events that have elapsed
        COTmrProcess(&canNode.Tmr);

        time::wait(50);
    }
}
