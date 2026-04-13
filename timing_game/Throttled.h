#pragma once
#include <functional>

// Luokka jonka avulla voi kutsua annettua funktiota tietyin aikavälein
class Throttled {
  public:
    // Suoritetaan annettu funktio, jos edellisestä suorituksesta 
    // on tarpeeksi pitkä aika
    void execute(uint32_t interval, const std::function<void()>& f) {
      uint32_t currentTime = millis();
      
      if ((currentTime - m_lastPressTime) >= interval) {
        m_lastPressTime = currentTime;
        f();
      }
    }
  private:
    uint32_t m_lastPressTime = 0;
};