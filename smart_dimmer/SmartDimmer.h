#pragma once
#include <memory>
#include "Light.h"

// "Fiksu" himmenninluokka
// Ottaa vastaan riippuvuusinjektiona lampun mitä himmennetään, sekä himmennysvälin.
// Käskee vastaanottamansa lamppua kirkastumaan ja himmentymään haluttaessa
class SmartDimmer {
  public:
    enum LastOperation {
      DIM, BRIGHTEN
    };

    // Konstruktori
    SmartDimmer(Light& light, const uint32_t dimmingInterval, const uint32_t dimmingPercentage) 
      : m_light(light), m_dimmingInterval(dimmingInterval), m_dimmingPercentage(dimmingPercentage), m_lastOp(DIM)
    {}

    // Loopissa kuunnellaan onko aika himmentää tai kirkastaa lamppua, ja toimitaan sen mukaan
    void loop() {
      if (m_timeToChangeBrightness) handleTimeToChangeBrightness();
      if (m_timeToStopBrightnessChange) handleTimeToStopBrightnessChange();

      // Kirkastetaan tai himmennetään lamppua niin kauan kun nappi on painettu
      if (longPress() && m_light.isOn()) {
        switch (m_lastOp) {
          case DIM:
            m_light.increaseBrightness(m_dimmingInterval, m_dimmingPercentage);
            break;
          case BRIGHTEN:
            m_light.decreaseBrightness(m_dimmingInterval, m_dimmingPercentage);
            break;
        }
        // Jos havaitaan nopea napin painallus, muutetaan lampun tilaa
      } else if (m_btnReleasedTime > 0 && m_timeToSwitchLampState) {
        m_light.switchLightState();
        m_lastOp = DIM; // Vaihdetaan viimeinen operaatio takaisin himmennykseksi, että eka muutos on kirkastus
        m_timeToSwitchLampState = false;
      }
    }

    // Painaessa nappia, asetetaan flagi, että nyt on aika muuttaa kirkkautta
    void startBrightnessChange() {
      m_timeToChangeBrightness = true;
    }

    // Nostaessa sormen napilta, asetetaan flägi, että nyt olisi aika lopettaa kirkkauden muunnos
    void stopBrightnessChange() {
      m_timeToStopBrightnessChange = true;
    }

  private:
    static constexpr uint32_t LAMP_STATE_SWITCH_DELAY = 300;
    static constexpr uint32_t CLICK_BOUNCE_TIME = 30;

    // Viite valoon, jota halutaan manipuloida
    Light& m_light;
    uint32_t m_dimmingInterval = 0;
    uint32_t m_dimmingPercentage = 0;
    LastOperation m_lastOp = DIM;
    uint32_t m_btnPressedDownTime = UINT32_MAX;
    uint32_t m_btnReleasedTime = UINT32_MAX / 2;
    bool m_timeToSwitchLampState = false;

    // Tilamuuttujat joita muutetaan keskeytyksillä
    volatile bool m_timeToChangeBrightness = false;
    volatile bool m_timeToStopBrightnessChange = false;

    // Alustetaan napin painamisaika tällä hetkellä ja nollataan napin nostoaika
    void handleTimeToChangeBrightness() {
      m_btnPressedDownTime = millis();
      m_btnReleasedTime = 0;
      m_timeToChangeBrightness = false;
    }

    // Kun sormi on nostettu napilta, tapahtuu kaksi asiaa:
    //   Jos nopea napsautus, vaihdetaan lampun tilaa
    //   Jos pitkä painallus, vaihdetaan edellistä operaatiota, jotta seuraavalla kerralla tiedetään 
    //   himmennetäänkö vai kirkastetaanko
    void handleTimeToStopBrightnessChange() {
      m_btnReleasedTime = millis();

      if (quickClick()) {
        m_timeToSwitchLampState = true;
      } else {
        m_lastOp = (m_lastOp == BRIGHTEN) ? DIM : BRIGHTEN;
        m_btnReleasedTime = 0;
      }

      m_btnPressedDownTime = 0;
      m_timeToStopBrightnessChange = false;
    }

    // Nappi on pohjassa, jos pohjaanpainon aikaleima on suurempaa kuin 0, ja vapautusta ei ole asetettu
    bool longPress() {
      return m_btnPressedDownTime > 0 && m_btnReleasedTime == 0;
    }

    // Nopea painallus:
    //   1. Painalluksen ja noston välinen aika tarpeeksi pieni,
    //   2. Mutta ei liian pieni, jotta suodatetaan kohinan aiheuttamat painallukset
    bool quickClick() {
      uint32_t duration = m_btnReleasedTime - m_btnPressedDownTime;
      return ((m_btnReleasedTime - m_btnPressedDownTime) < LAMP_STATE_SWITCH_DELAY) && (duration > CLICK_BOUNCE_TIME);
    }
};