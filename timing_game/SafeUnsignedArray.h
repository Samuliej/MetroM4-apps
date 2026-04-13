#pragma once
#include <memory>
#include <cstdint>

// Turvallinen taulukkotyyppi joka käsittelee vain etumerkittömiä lukuja
struct SafeUnsignedArray {
  public:
    SafeUnsignedArray(uint32_t arrSize) : arrSize(arrSize), elements(new uint32_t[arrSize]) {
      // Alustetaan elementit nollaksi
      for (uint32_t i = 0; i < arrSize; i++) {
        elements[i] = 0;
      }
    }

    // Estetään kirjoitus ja luku rajojen yli
    uint32_t& operator[](uint32_t index) {
      if (index >= arrSize) {
        return errorReturnValue;
      }

      return elements[index];
    }

    // Estetään pelkkä luku rajojen yli
    const uint32_t& operator[](uint32_t index) const {
      if (index >= arrSize) {
        return errorReturnValue;
      }

      return elements[index];
    }

    uint32_t size() const { return arrSize; }
    uint32_t last() const { return elements[size() - 1]; }
    uint32_t first() const { return elements[0]; }

    // Estetään kopiointi
    // Nämä ovat todennäköisesti turhat, sillä oman ymmärryksen mukaan, jos luokka
    // tai structi sisältää jonkin jäsenmuuttujan mitä ei voi kopioida, niin kääntäjä 
    // poistaa copy constructorit myös jäsenmuuttujan omistavalta luokalta.
    // C++ speksi on kuitenkin aika kankeaa luettavaa, joten jätän ne tähän.
    SafeUnsignedArray(const SafeUnsignedArray&) = delete;
    SafeUnsignedArray& operator=(const SafeUnsignedArray&) = delete;
  private:
    const uint32_t arrSize;
    std::unique_ptr<uint32_t[]> elements;
    uint32_t errorReturnValue = 0; // muuten esim -1 jos ei käytettäisi etumerkittömiä inttejä
};
