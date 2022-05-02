#include <EVT/io/CAN.hpp>
#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/utils/time.hpp>
#include <EVT/utils/types/FixedQueue.hpp>
#include <EVT/io/CANopen.hpp>
#include <EVT/dev/platform/f3xx/f302x8/Timerf302x8.hpp>

#include <Canopen/co_core.h>

#include <charge_controller/ChargeController.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <charge_controller/dev/Debounce.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>
#include <charge_controller/Logger.h>


namespace IO = EVT::core::IO;
namespace DEV = EVT::core::DEV;
namespace time = EVT::core::time;

// IO pins
constexpr IO::Pin RELAY_CTL_PIN = IO::Pin::PB_0;
constexpr IO::Pin LED_PIN = IO::Pin::PA_10;
constexpr IO::Pin STANDBY_BUTTON_PIN = IO::Pin::PA_2;
constexpr IO::Pin START_BUTTON_PIN = IO::Pin::PA_7;
constexpr IO::Pin BATTERY_1_OK = IO::Pin::PA_9;
constexpr IO::Pin BATTERY_2_OK = IO::Pin::PA_8;

// LCD pins
constexpr IO::Pin LCD_A0_PIN = IO::Pin::PA_6;
constexpr IO::Pin LCD_RST_PIN = IO::Pin::PA_1;
constexpr IO::Pin SPI_CS_PIN = IO::Pin::PA_3;
constexpr IO::Pin SPI_MOSI_PIN = IO::Pin::PB_5;
constexpr IO::Pin SPI_CLK_PIN = IO::Pin::PA_5;

// UART pins
constexpr IO::Pin UART_RX_PIN = IO::Pin::PB_7;
constexpr IO::Pin UART_TX_PIN = IO::Pin::PB_6;

// CAN pins
constexpr IO::Pin CAN_RX_PIN = IO::Pin::PA_11;
constexpr IO::Pin CAN_TX_PIN = IO::Pin::PA_12;

struct CANInterruptParams {
    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage>* queue;
    BMSManager* bmsManager;
};

/**
 * Interrupt handler to get CAN messages. A function pointer to this function
 * will be passed to the EVT-core CAN interface which will in turn call this
 * function each time a new CAN message comes in.
 *
 * NOTE: For this sample, every non-extended (so 11 bit CAN IDs) will be
 * assummed to be intended to be passed as a CANopen message.
 *
 * @param message[in] The passed in CAN message that was read.
 */
void canInterrupt(IO::CANMessage &message, void *priv) {
    struct CANInterruptParams* params = (CANInterruptParams*) priv;

    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage> *queue = params->queue;
    BMSManager* bms = params->bmsManager;
    if (queue != nullptr)
        queue->append(message);
}

/////////////////////////////////////////////////////////////////////////////////
//// CANopen specific Callbacks. Need to be defined in some location
/////////////////////////////////////////////////////////////////////////////////
//extern "C" void CONodeFatalError(void) {}
//
//extern "C" void COIfCanReceive(CO_IF_FRM *frm) {}
//
//extern "C" int16_t COLssStore(uint32_t baudrate, uint8_t nodeId) { return 0; }
//
//extern "C" int16_t COLssLoad(uint32_t *baudrate, uint8_t *nodeId) { return 0; }
//
//extern "C" void CONmtModeChange(CO_NMT *nmt, CO_MODE mode) {}
//
//extern "C" void CONmtHbConsEvent(CO_NMT *nmt, uint8_t nodeId) {}
//
//extern "C" void CONmtHbConsChange(CO_NMT *nmt, uint8_t nodeId, CO_MODE mode) {}
//
//extern "C" int16_t COParaDefault(CO_PARA *pg) { return 0; }
//
//extern "C" void COPdoTransmit(CO_IF_FRM *frm) {}
//
//extern "C" int16_t COPdoReceive(CO_IF_FRM *frm) { return 0; }
//
//extern "C" void COPdoSyncUpdate(CO_RPDO *pdo) {}
//
//extern "C" void COTmrLock(void) {}
//
//extern "C" void COTmrUnlock(void) {}

int main() {
    IO::init();

    // Will store CANopen messages that will be populated by the EVT-core CAN
    // interrupt
    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage> canOpenQueue;

    // Pin initialization
    IO::UART &uart = IO::getUART<UART_TX_PIN, UART_RX_PIN>(9600);
    IO::CAN &can = IO::getCAN<CAN_TX_PIN, CAN_RX_PIN>();

    // GPIO pins
    IO::GPIO &relayControl =
            IO::getGPIO<RELAY_CTL_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &statusLED = IO::getGPIO<LED_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &batt1OK = IO::getGPIO<BATTERY_1_OK>(IO::GPIO::Direction::INPUT);
    IO::GPIO &batt2OK = IO::getGPIO<BATTERY_2_OK>(IO::GPIO::Direction::INPUT);

    // Buttons
    Debounce standbyButton(
            IO::getGPIO<STANDBY_BUTTON_PIN>(IO::GPIO::Direction::INPUT));
    Debounce startButton(
            IO::getGPIO<START_BUTTON_PIN>(IO::GPIO::Direction::INPUT));
    // LCD
    IO::GPIO &LCDRegisterSEL =
            IO::getGPIO<LCD_A0_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &LCDReset = IO::getGPIO<LCD_RST_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO *devices[] = {&IO::getGPIO<SPI_CS_PIN>(IO::GPIO::Direction::OUTPUT)};
    devices[0]->writePin(IO::GPIO::State::HIGH);

    IO::SPI &spi = IO::getSPI<SPI_CLK_PIN, SPI_MOSI_PIN>(devices, 1);

    // charge controller module instantiation
    BMSManager bms(can, BMS_CAN_ADDRESS);
    LCDDisplay display(spi, LCDRegisterSEL, LCDReset);
    ChargeController chargeController(bms, display, relayControl);

    struct CANInterruptParams canParams = {
            .queue = &canOpenQueue,
            .bmsManager = &bms,
    };
    can.addIRQHandler(canInterrupt, reinterpret_cast<void*>(&canParams));
    DEV::Timerf302x8 timer(TIM2, 100);

//    // Reserved memory for CANopen stack usage
//    uint8_t sdoBuffer[1][CO_SDO_BUF_BYTE];
//    CO_TMR_MEM appTmrMem[4];
//
//    CO_IF_DRV canStackDriver;
//    CO_IF_CAN_DRV canDriver;
//    CO_IF_TIMER_DRV timerDriver;
//    CO_IF_NVM_DRV nvmDriver;
//    IO::getCANopenCANDriver(&can, &canOpenQueue, &canDriver);
//    IO::getCANopenTimerDriver(&timer, &timerDriver);
//    IO::getCANopenNVMDriver(&nvmDriver);
//
//    canStackDriver.Can = &canDriver;
//    canStackDriver.Timer = &timerDriver;
//    canStackDriver.Nvm = &nvmDriver;
//
//    CO_NODE_SPEC canSpec = {
//        .NodeId = 0x01,
//        .Baudrate = IO::CAN::DEFAULT_BAUD,
//        .Dict = testCanNode.getObjectDictionary(),
//        .DictLen = testCanNode.getNumElements(),
//        .EmcyCode = NULL,
//        .TmrMem = appTmrMem,
//        .TmrNum = 16,
//        .TmrFreq = 100,
//        .Drv = &canStackDriver,
//        .SdoBuf = reinterpret_cast<uint8_t*>(&sdoBuffer[0]),
//    };
//
//    CO_NODE canNode;
//    time::wait(500);
//
//    can.connect();
//
//    CONodeInit(&canNode, &canSpec);
//    CONodeStart(&canNode);
//    CONmtSetMode(&canNode.Nmt, CO_OPERATIONAL);

    display.init();
    chargeController.init();

    while (true) {
        chargeController.loop();
        if (standbyButton.read())
            chargeController.stopCharging();

        if (startButton.read())
            chargeController.startCharging();

        time::wait(50);
    }
}
