#include <charge_controller/dev/Debounce.hpp>

Debounce::Debounce(EVT::core::IO::GPIO &pin) : pin(pin) {
  lastState = pin.readPin();
  state = lastState;
}

bool Debounce::read() {
  if (pin.readPin() != lastState) {
    lastPressed = time::millis();
    lastState = pin.readPin();
  } else if (time::millis() - lastPressed > debounceTime) {
    state = lastState;
  }

  return state == pressedState;
}