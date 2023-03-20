#include <EVT/io/CAN.hpp>
#include <EVT/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/utils/time.hpp>

#include <charge_controller/ChargeController.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <charge_controller/dev/Debounce.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>

namespace IO = EVT::core::IO;
namespace time = EVT::core::time;

#define MAX_VOLTAGE 12
#define MAX_TEMPERATURE 90.0

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

const uint32_t SPI_SPEED = SPI_SPEED_500KHZ;
const uint8_t deviceCount = 1;

int main() {
    EVT::core::platform::init();

    constexpr uint8_t BMS_CAN_ADDRESS = 0x00;

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
    IO::GPIO* devices[deviceCount];

    IO::GPIO& LCDRegSelect = IO::getGPIO<IO::Pin::PA_3>(EVT::core::IO::GPIO::Direction::OUTPUT);
    IO::GPIO& LCDReset = IO::getGPIO<IO::Pin::PB_3>(EVT::core::IO::GPIO::Direction::OUTPUT);
    devices[0] = &IO::getGPIO<IO::Pin::PB_12>(EVT::core::IO::GPIO::Direction::OUTPUT);
    devices[0]->writePin(IO::GPIO::State::HIGH);
    auto& spi = IO::getSPI<IO::Pin::SPI_SCK, IO::Pin::SPI_MOSI>(devices, deviceCount);
    spi.configureSPI(SPI_SPEED, SPI_MODE0, SPI_MSB_FIRST);

    // charge controller module instantiation
    BMSManager bms(can, BMS_CAN_ADDRESS);
    LCDDisplay display(LCDRegSelect, LCDReset, spi);
    ChargeController chargeController(bms, display, relayControl);

    display.init();
    chargeController.init();

    display.showSections();
    while (1) {
        chargeController.loop();
        if (standbyButton.read())
            chargeController.stopCharging();

        if (startButton.read())
            chargeController.startCharging();

        time::wait(50);
    }
}
