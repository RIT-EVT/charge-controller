#include <EVT/utils/log.hpp>
#include <charge_controller/dev/Debounce.hpp>

namespace log = EVT::core::log;

Debounce::Debounce(EVT::core::IO::GPIO& pin) : pin(pin) {
    lastState = pin.readPin();
    state = lastState;
}

bool Debounce::read() {
    if (pin.readPin() != lastState) {
        lastPressed = time::millis();
        lastState = pin.readPin();
        isPressed = false;
    } else if (time::millis() - lastPressed > debounceTime) {
        state = lastState;
        isPressed = true;
    }

    return state == pressedState;
}
