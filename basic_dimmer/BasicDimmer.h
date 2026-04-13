#pragma once
#include <memory>
#include "Light.h"

// Himmenninluokka
// Ottaa vastaan riippuvuusinjektiona lampun mitä himmennetään, sekä himmennysvälin.
// Käskee vastaanottamansa lampun kirkastumaan ja himmentymään tarvittaessa
class BasicDimmer {
  public:
    BasicDimmer(Light& light, const uint32_t dimmingInterval, const uint32_t dimmingPercentage) 
      : m_light(light), m_dimmingInterval(dimmingInterval), m_dimmingPercentage(dimmingPercentage)
    {}

    // Loopissa kuunnellaan onko aika himmentää tai kirkastaa lamppua, ja toimitaan sen mukaan
    void loop() {
      if (m_timeToInc) m_light.increaseBrightness(m_dimmingInterval, m_dimmingPercentage);
      if (m_timeToDec) m_light.decreaseBrightness(m_dimmingInterval, m_dimmingPercentage);
    }

    void startIncreasingBrightness() {
      m_timeToInc = true;
      m_timeToDec = false;
    }

    void stopIncreasingBrightness() {
      m_timeToInc = false;
    }

    void startDecreasingBrightness() {
      m_timeToDec = true;
      m_timeToInc = false;
    }

    void stopDecreasingBrightness() {
      m_timeToDec = false;
    }

  private:
    Light& m_light;
    uint32_t m_dimmingInterval = 0;
    uint32_t m_dimmingPercentage = 0;
    volatile bool m_timeToInc = false; // Muuttujat joita muutetaan keskeytyskäskyn kautta
    volatile bool m_timeToDec = false;
};