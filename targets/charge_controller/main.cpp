// clang-format off
#include <EVT/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/utils/log.hpp>
#include <EVT/utils/time.hpp>
#include <EVT/utils/types/FixedQueue.hpp>
#include <EVT/dev/platform/f3xx/Timerf3xx.hpp> // TODO: clang will put this at the top causing compilation problem
// clang-format on

#include <EVT/io/CANopen.hpp>

#include <EVT/dev/button.hpp>
#include <charge_controller/ChargeController.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <charge_controller/dev/LCDView.hpp>

namespace IO = EVT::core::IO;
namespace DEV = EVT::core::DEV;
namespace time = EVT::core::time;
namespace log = EVT::core::log;
namespace platform = EVT::core::platform;

// IO pins
constexpr IO::Pin STANDBY_BUTTON_PIN = IO::Pin::PA_4;
constexpr IO::Pin START_BUTTON_PIN = IO::Pin::PA_6;
constexpr IO::Pin RESET_BUTTON_PIN = IO::Pin::PA_1;

constexpr IO::Pin ENCODER_BUTTON_PIN = IO::Pin::PB_4;
constexpr IO::Pin ENCODER_A_PIN = IO::Pin::PA_8;
constexpr IO::Pin ENCODER_B_PIN = IO::Pin::PA_9;

constexpr IO::Pin BATTERY_1_OK_PIN = IO::Pin::PB_6;
constexpr IO::Pin BATTERY_2_OK_PIN = IO::Pin::PB_5;

constexpr IO::Pin LCD_REGISTER_SELECT_PIN = IO::Pin::PB_0;
constexpr IO::Pin LCD_RESET_PINT = IO::Pin::PA_1;
constexpr IO::Pin LCD_SPI_DEVICE_0_PIN = IO::Pin::PA_0;

constexpr IO::Pin LCD_SPI_SCK = IO::Pin::PA_5;
constexpr IO::Pin LCD_SPI_MOSI = IO::Pin::PA_7;

constexpr IO::Pin CAN_TX = IO::Pin::PA_12;
constexpr IO::Pin CAN_RX = IO::Pin::PA_11;

constexpr IO::Pin LED_PIN = IO::Pin::PA_10;

constexpr uint32_t SPI_SPEED = SPI_SPEED_500KHZ;

///////////////////////////////////////////////////////////////////////////////
// EVT-core CAN callback and CAN setup. This will include logic to set
// aside CANopen messages into a specific queue
///////////////////////////////////////////////////////////////////////////////

struct CANInterruptParams {
    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage>* queue;
    CC::ChargeController* chargeController;
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

    if (message.getId() == CHARGER_STATUS_CAN_ID) {
        // Display the received current and voltage from the charger.
        uint16_t voltage = ((uint16_t) message.getPayload()[0] << 8) | message.getPayload()[1];
        uint16_t current = ((uint16_t) message.getPayload()[2] << 8) | message.getPayload()[3];

        params->chargeController->setDisplayChargerValues(voltage, current);
    }

    if (queue != nullptr)
        queue->append(message);
}

///////////////////////////////////////////////////////////////////////////////
// CANopen specific Callbacks. Need to be defined in some location
///////////////////////////////////////////////////////////////////////////////

extern "C" void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef* hcan) {
    log::LOGGER.log(log::Logger::LogLevel::DEBUG, "RX Full");
}

IO::GPIO* devices[1];

int main() {
    platform::init();

    // Pin initialization
    IO::UART& uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(9600, true);
    uart.printf("Uart Init\n\r");
    log::LOGGER.setUART(&uart);

    // GPIO pins
    IO::GPIO& statusLED = IO::getGPIO<LED_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO* bmsOK[] = {
        &IO::getGPIO<BATTERY_1_OK_PIN>(IO::GPIO::Direction::INPUT),
        &IO::getGPIO<BATTERY_2_OK_PIN>(IO::GPIO::Direction::INPUT)};
    uart.printf("GPIO Init\n\r");

    // Buttons
    IO::GPIO& startButtonGPIO = IO::getGPIO<START_BUTTON_PIN>(IO::GPIO::Direction::INPUT);

    //DEV::Button standbyButton = DEV::Button(standbyButtonGPIO);
    DEV::Button startButton(startButtonGPIO);

    uart.printf("Buttons Init\n\r");

    // LCD
    IO::GPIO& LCDRegisterSEL = IO::getGPIO<LCD_REGISTER_SELECT_PIN>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO& LCDReset = IO::getGPIO<LCD_RESET_PINT>(IO::GPIO::Direction::OUTPUT);

    devices[0] = &IO::getGPIO<LCD_SPI_DEVICE_0_PIN>(EVT::core::IO::GPIO::Direction::OUTPUT);
    devices[0]->writePin(IO::GPIO::State::HIGH);

    uart.printf("LCD Init\n\r");

    // Spi
    IO::SPI& spi = IO::getSPI<LCD_SPI_SCK, LCD_SPI_MOSI>(devices, 1);
    spi.configureSPI(SPI_SPEED, SPI_MODE0, SPI_MSB_FIRST);
    uart.printf("Spi Init\n\r");

    // Initialize CAN, add an IRQ which will add messages to the queue above
    IO::CAN& can = IO::getCAN<CAN_TX, CAN_RX>();

    // charge controller module instantiation
    CC::BMSManager bms(can, bmsOK);

    //Model initialization
    CC::UIModel model;

    //LCD display initialization
    CC::LCDView display(LCDRegisterSEL, LCDReset, spi, model);

    //Encoder GPIO pin initialization
    IO::GPIO& encoderGPIOpinA = IO::getGPIO<ENCODER_A_PIN>(IO::GPIO::Direction::INPUT);
    IO::GPIO& encoderGPIOpinB = IO::getGPIO<ENCODER_B_PIN>(IO::GPIO::Direction::INPUT);
    DEV::Encoder encoder(encoderGPIOpinA, encoderGPIOpinB, 1, 0, true);
    //Encoder button
    IO::GPIO& encoderButtonGPIO = IO::getGPIO<ENCODER_BUTTON_PIN>(IO::GPIO::Direction::INPUT);
    DEV::Button encoderButton(encoderButtonGPIO, IO::GPIO::State::LOW);

    //UI Controller initialization
    CC::UIController controllerUI(encoder, encoderButton, display, model);
    CC::ChargeController chargeController(bms, display, can, startButton, statusLED, controllerUI, model);

    uart.printf("Modules Initialized\n\r");

    DEV::Timerf3xx timer(TIM2, 100);

    log::LOGGER.setLogLevel(log::Logger::LogLevel::DEBUG);

    display.init();
    chargeController.init();

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
        .chargeController = &chargeController};
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
        .NodeId = CC::BMSManager::NODE_ID,
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

        // Process the BMS RPDOs
        CONodeProcess(&canNode);
        // Update the state of timer based events
        COTmrService(&canNode.Tmr);
        // Handle executing timer events that have elapsed
        COTmrProcess(&canNode.Tmr);

        time::wait(50);
    }
}
