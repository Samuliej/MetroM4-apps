#pragma once
#include <cstdint>
#include "Throttled.h"

// Luokka joka pitää sisällään tiedon yksittäisestä LED -lampusta.
// Hoitaa itsensä kirkastamisen ja himmentämisen
class Light {
  public:
    Light(const uint8_t pin) : m_pin(pin) {
      pinMode(m_pin, OUTPUT);
      analogWrite(m_pin, MAX_BRIGHTNESS / INTERNAL_SCALE_FACTOR
      );
    }

    // Hyödynnetään Throttled luokkaa, jonka avulla voimme suorittaa annetun
    // funktion rajoitetun ajan sisällä
    void increaseBrightness(const uint32_t interval, const uint32_t dimmingPercentage) {
      m_throttled.execute(interval, [this, dimmingPercentage] {
        calculateAndSetNewBrightness(false, dimmingPercentage);
      });
    }

    void decreaseBrightness(const uint32_t interval, const uint32_t dimmingPercentage) {
      m_throttled.execute(interval, [this, dimmingPercentage] {
        calculateAndSetNewBrightness(true, dimmingPercentage);
      });
    }

  private:
    uint8_t m_pin;
    uint32_t m_brightness = 255000;  // Käytetään isompia uint arvoja, jotta voimme laskea 

    static constexpr uint32_t MIN_BRIGHTNESS = 100;  // Jos vähimmäishimmeys päästetään alle 100, niin calculateAndSetNewBrightness laskut hajoavat
    static constexpr uint32_t MAX_BRIGHTNESS = 255000;
    static constexpr uint32_t PERCENT_BASE = 100;
    static constexpr uint32_t INTERNAL_SCALE_FACTOR = 1000;

    Throttled m_throttled;

    // Lasketaan uusi kirkkaus. Käsittelee erotuksen ja lisäämisen
    void calculateAndSetNewBrightness(const bool substracting = false, const uint32_t dimmingPercentage = 5) {
      uint32_t change = (m_brightness / PERCENT_BASE) * dimmingPercentage; // Lisätty tai vähennetty prosenttiarvo
      uint32_t val;

      // Lisäys tai vähennys operaation tyypin mukaan
      if (substracting) {
        // Estetään uint muuttujan pyörähdys, muuten vähennetään muutos
        val = (change > m_brightness) ? MIN_BRIGHTNESS : m_brightness - change;
      } else {
        val = m_brightness + change;
      }

      m_brightness = constrain(val, MIN_BRIGHTNESS, MAX_BRIGHTNESS); // Rajataan arvo halutulle alueelle

      uint8_t ledValue = m_brightness / INTERNAL_SCALE_FACTOR;       // Annetaan ledille arvo väliltä 0-255
      analogWrite(m_pin, ledValue);                                 
    }
};