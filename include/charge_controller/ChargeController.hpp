#ifndef CHARGE_CONTROLLER_H
#define CHARGE_CONTROLLER_H

#include <EVT/io/CAN.hpp>
#include <EVT/io/GPIO.hpp>
#include <charge_controller/dev/BMSManager.hpp>
#include <charge_controller/dev/LCDDisplay.hpp>

#define MAX_VOLTAGE 12
#define MAX_TEMPERATURE 90.0

class ChargeController {
public:
  enum class ControllerStates {
    NO_BATTERY,
    CONNECTED,
    CHARGING,
    STANDBY,
    FAULT
  };
  ChargeController(BMSManager bms, LCDDisplay display, IO::GPIO &relay);
  void init();
  void loop();
  bool checkBMS();
  bool isCharging() { return state == ControllerStates::CHARGING; };
  bool hasFault() { return state == ControllerStates::FAULT; };

  void startCharging();
  void stopCharging();

private:
  void noBatteryState();
  void connectedState();
  void chargingState();
  void standbyState();
  void faultState();

  BMSManager bms;
  LCDDisplay display;
  IO::GPIO &relay;

  ControllerStates state = ControllerStates::NO_BATTERY;
  bool changedState = true;

  static constexpr IO::GPIO::State RELAY_ON = IO::GPIO::State::HIGH;
  static constexpr IO::GPIO::State RELAY_OFF = IO::GPIO::State::LOW;
};

#endif // CHARGE_CONTROLLER_H
