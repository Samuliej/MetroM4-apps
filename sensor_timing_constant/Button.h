#pragma once

// Tietorakenne pitämään napin tietoja
struct Button {
  uint8_t pin;
  uint8_t ISRState;
  
  // Apufunktiot joiden avulla haemme napin tilaa
  uint8_t isPressed() {
    return digitalRead(pin) == LOW;
  }

  uint8_t isReleased() {
    return digitalRead(pin) == HIGH;
  }
};