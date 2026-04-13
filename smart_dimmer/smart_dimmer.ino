#include "SmartDimmer.h"
#include "Light.h"
#include "Button.h"
#include <memory>

constexpr uint8_t B1_BTN = 3;
constexpr uint8_t YELLOW_LED = 5;
constexpr uint32_t DIMMING_INTERVAL = 100; // Väli millisekunteina kuinka usein lampun kirkkautta muutetaan
constexpr uint32_t DIMMING_PERCENTAGE = 5; 

// unique_ptr:t Luotuihin luokkiin
std::unique_ptr<SmartDimmer> dimmer;
std::unique_ptr<Light> light;

Button increaseBrightnessBtn;

void setup() {
  Serial.begin(9600); // Alustetaan sarjamonitori max 9600 bps
  while(!Serial);     // Odotetaan että sarjamonitori käynnistyy

  initButton(increaseBrightnessBtn, B1_BTN, CHANGE);

  light = std::unique_ptr<Light>(new Light(YELLOW_LED));
  // Välitetään himmentimelle viite light olioon
  dimmer = std::unique_ptr<SmartDimmer>(new SmartDimmer(*light, DIMMING_INTERVAL, DIMMING_PERCENTAGE));

  // Liitetään keskeytykset jotka kuuntelevat napin muutosta
  attachInterrupt(digitalPinToInterrupt(increaseBrightnessBtn.pin), increaseBrightnessISR, increaseBrightnessBtn.ISRState);
}

void loop() {
  dimmer->loop();
}

// Keskeytysrutiinit taas vain flippaavat flägit, jotka ilmaisevat 
// että nyt jotain pitää tehdä
void increaseBrightnessISR() {
  if (increaseBrightnessBtn.isPressed()) dimmer->startBrightnessChange();
  if (increaseBrightnessBtn.isReleased()) dimmer->stopBrightnessChange();
}

void initButton(Button& btn, const uint8_t pin, const uint8_t state) {
  btn.pin = pin;
  btn.ISRState = state;
  pinMode(btn.pin, INPUT);
}
