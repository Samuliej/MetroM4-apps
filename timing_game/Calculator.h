#pragma once
#include <cmath>
#include "SafeUnsignedArray.h"
#include <memory>
#include <cstdint>

// Luokka joka hoitaa TimingGame:n laskutoimitukset
class Calculator {
  private:
    static constexpr float MS_TO_S = 1000.0f; // Vakio joka määrittyy jo käännösvaiheessa
  public:
    // Lasketaan painallusten kokonaisaika
    static float calculateTotalSeconds(const SafeUnsignedArray& arr) {
      if (arr.size() < 2) return 0.0f;
      return calculateTotalMs(arr) / MS_TO_S;
    }

    // Taulukko sisältää aikaleimoja, joten riittää laskea viimeisen ja ensimmäisen erotus
    static float calculateTotalMs(const SafeUnsignedArray& arr) {
      if (arr.size() < 2) return 0.0f;
      return (float)(arr.last() - arr.first());
    }

    // Lasketaan painallusten keskiarvo
    static float calculateAverage(const SafeUnsignedArray& arr) {
      if (arr.size() < 2) return 0.0f;
      return (calculateTotalSeconds(arr) / (float)(arr.size() - 1)); // Käytetään arr.size() - 1, koska lasketaan painallusten välejä
    }

    // Lasketaan painallusten keskihajonta
    static float calculateSTD(const SafeUnsignedArray& arr) {
      if (arr.size() < 2) return 0.0f;
      
      float avg = calculateTotalMs(arr) / (float)(arr.size() - 1);
      float squaredDiffsSum = 0.0f;

      for (uint8_t i = 1; i < arr.size(); i++) {
        float interval = (float)(arr[i] - arr[i - 1]);
        float diff = interval - avg;
        squaredDiffsSum += (diff * diff);
      }

      float variance = squaredDiffsSum / (float)(arr.size() - 1);
      return std::sqrt(variance) / MS_TO_S;
    }
};