#pragma once
#include "Logger.h"
#include "Calculator.h"
#include "SafeUnsignedArray.h"
#include "Throttled.h"
#include <memory>
#include <cstdint>
#include <cmath>

class TimingGame {
  public:
    enum class State {
      IN_PROGRESS,
      GAME_OVER
    };

    TimingGame(const float goalTime, const uint8_t requiredPresses, Print& serial) 
      : m_goalTime(goalTime), m_intervals((uint32_t)requiredPresses), m_stream(serial) {}

    void play() {
      handleGameReset();

      if (m_state == State::GAME_OVER) return;

      handleCalculations();
      handleClickPrint();
      handleGameEnd();
    }

    void reset() {
      m_timeToReset = true;
    }

    void setTimeToCalculate(const bool b) { m_timeToCalculate = b; }

  private:
    // Pelin vakiot
    const float MIN_BTN_PRESS_TIME = 300.0f;

    // Pelin arvomuuttujat
    float m_goalTime = 0.0f;
    uint32_t m_lastPressTime = 0;
    uint8_t m_currentPressIndex = 0;

    // Pelin tilamuuttujat
    State m_state = State::IN_PROGRESS;
    bool m_printNum = false;
    volatile bool m_timeToReset = false;     // volatile, koska näitä muutetaan pääohjelmassa tapahtuvalla keskeytyksellä
    volatile bool m_timeToCalculate = false; 

    // Tietorakenteet
    SafeUnsignedArray m_intervals;
    Throttled throttled;

    // Riippuvuudet
    Print& m_stream; // Viite Serial:iin jonka kautta printataan

    bool timeToCalculate() {
      return m_timeToCalculate == true;
    }

    bool isFinished() const {
      return m_currentPressIndex >= m_intervals.size();
    }

    void calculatePressEvent() {
      uint32_t currentPressTime = millis();

      if (isFinished()) return;

      m_intervals[m_currentPressIndex++] = currentPressTime;
      m_lastPressTime = currentPressTime;
      m_printNum = true;
    }

    void handleClickPrint() {
      if (!m_printNum) return;

      Logger::logSinglePress(m_stream, m_currentPressIndex);
      m_printNum = false;
    }

    // Lasketaan ja printataan tulokset
    void handleGameEnd() {
      if (m_state == State::IN_PROGRESS && isFinished()) {
        float result  = Calculator::calculateTotalSeconds(m_intervals);
        float error   = std::fabs(m_goalTime - result);
        float average = Calculator::calculateAverage(m_intervals);
        float std     = Calculator::calculateSTD(m_intervals);

        Logger::logResult(m_stream, m_goalTime, result, error);
        Logger::logAverage(m_stream, average);
        Logger::logSTD(m_stream, std);

        m_stream.print("Voit alustaa pelin B2 näppäimellä.\n\n");
        m_state = State::GAME_OVER;
      }
    }

    void handleCalculations() {
      if (timeToCalculate()) {
        m_timeToCalculate = false;
        throttled.execute(MIN_BTN_PRESS_TIME, [this] {
          calculatePressEvent();
        });
      }
    }

    void handleGameReset() {
      if (!m_timeToReset) return;

      m_lastPressTime = 0;
      m_currentPressIndex = 0;

      m_printNum = false;
      m_timeToCalculate = false;
      m_timeToReset = false;

      m_state = State::IN_PROGRESS;

      delay(100);
      m_stream.print("Peli alustettu. Voit painaa B1 näppäintä aloittaaksesi.\n\n");
    }
};