#ifndef DEBOUNCE_HPP
#define DEBOUNCE_HPP

#include <EVT/io/GPIO.hpp>
#include <EVT/utils/time.hpp>
#include <cstdint>

namespace time = EVT::core::time;
namespace IO = EVT::core::IO;

class Debounce {
public:
  static constexpr uint32_t DEFAULT_TIME = 50;

  Debounce(IO::GPIO &pin);

  void setPressedState(IO::GPIO::State pressedState) {
    this->pressedState = pressedState;
  };

  void setDebounceTime(uint32_t ms) { this->debounceTime = ms; };

  bool read();

private:
  uint32_t debounceTime = DEFAULT_TIME;
  uint32_t lastPressed = time::millis();

  IO::GPIO &pin;

  IO::GPIO::State pressedState = IO::GPIO::State::HIGH;
  IO::GPIO::State lastState;
  IO::GPIO::State state;
};

#endif // DEBOUNCE_HPP
