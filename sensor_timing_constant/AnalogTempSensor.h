#pragma once

// Yksinekertainen luokka, joka kuvastaa yhtä analogista mittaria 
class AnalogTempSensor {
  public: 
    AnalogTempSensor(const uint8_t powerPin, const uint8_t readPin) : m_powerPin(powerPin), m_readPin(readPin) {
      pinMode(m_powerPin, OUTPUT);
      pinMode(m_readPin, INPUT);
    }

    ~AnalogTempSensor() {
      powerOff();
    }

    void powerOn() {
      digitalWrite(m_powerPin, HIGH);
    } 

    void powerOff() {
      digitalWrite(m_powerPin, LOW);
    } 

    uint32_t measure() {
      return analogRead(m_readPin);
    }

  private:
    uint8_t m_powerPin = 0;
    uint8_t m_readPin = 0;
};