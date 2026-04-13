#include "TimingGame.h"
#include <memory>
// Joka luokkaan includattu eksplisiittisesti tarvittavat kirjastot.
// Esikääntäjä hoitaa ettei niitä turhaan sisällytetä useaa kertaa, kunhan 
// ifndef tai pragma once:t ovat kunnossa

// Huomioita: 
//   Alussa ihan kunnon karvalakkitoteutus, jossa kaikki logiikka
//   yhdessä tiedostossa vei 2% Metro M4 levyn muistista.
//   Järkevämmän arkkitehtuurin ja muistinkäytön refaktorointi 
//   nosti käytetyn muistin 3%. Ihan siedettävä lisäys.
//   std::unique_ptr:llä kuitenkin käytännössä olematon overhead.

const uint8_t B1_BTN = 3;
const uint8_t B2_BTN = 2;

const float GOAL_TIME = 20.0f;
const uint8_t REQUIRED_CLICKS = 5;

// Tietorakenne pitämään napin tietoja
struct Button {
  uint8_t pin;
  uint8_t ISRState;
};

std::unique_ptr<TimingGame> game;

void setup() {
  Serial.begin(9600); // Alustetaan sarjamonitori max 9600 bps
  while(!Serial);     // Odotetaan että sarjamonitori käynnistyy

  // Luodaan uusi TimingGame -instanssi
  // Serial välitetään dependancy injektionina 
  game = std::unique_ptr<TimingGame>(new TimingGame(GOAL_TIME, REQUIRED_CLICKS, Serial));

  // Alustetaan napit
  Button playBtn;
  Button resetBtn;
  initButton(playBtn, B1_BTN);
  initButton(resetBtn, B2_BTN);

  // Liitetään keskeytykset nappeihin
  attachInterrupt(digitalPinToInterrupt(playBtn.pin), playButtonPressedISR, playBtn.ISRState);
  attachInterrupt(digitalPinToInterrupt(resetBtn.pin), resetButtonPressedISR, resetBtn.ISRState);
}

void loop() {
  game->play();
}

// Keskeytykset vain flippaavat flagit pelin sisällä
void playButtonPressedISR() {
  game->setTimeToCalculate(true);
}

void resetButtonPressedISR() {
  game->reset();
}

void initButton(Button& btn, const uint8_t pin) {
  btn.pin = pin;
  btn.ISRState = RISING;
  pinMode(btn.pin, INPUT);
}