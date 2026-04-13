#include "Button.h"
#include "AnalogTempSensor.h"
#include <memory>

// Pienet funktioaliakset, jotta printtaus on hieman helpompaa
#define p(x) Serial.print(x);
#define pn(x) Serial.println(x);

// Vakiot
constexpr uint8_t B2_BTN = 2;
constexpr uint8_t SENSOR_POWER = A2;
constexpr uint8_t SENSOR_READ = A1;
constexpr uint8_t MEASUREMENTS_COUNT = 50;
constexpr uint8_t MEASUREMENT_DELAY = 50;

Button onBtn;
std::unique_ptr<AnalogTempSensor> tempSensor;
volatile bool timeToMeasure = false;
uint32_t resultArr[MEASUREMENTS_COUNT];

struct MeasurementTimes { // Tietorakenne pitämään mittauksen tietoja
  uint32_t tbp;   // Aika ennen virtojen päälle kytkemistä
  uint32_t fmt;   // Aika ensimmäisen mittauksen jälkeen  
  uint32_t start; // Mittausten aloitusaika
  uint32_t end;   // Mittausten lopetusaika
};

void setup() {
  Serial.begin(9600); // Alustetaan sarjamonitori max 9600 bps
  while(!Serial);     // Odotetaan että sarjamonitori käynnistyy

  tempSensor = std::unique_ptr<AnalogTempSensor>(new AnalogTempSensor(SENSOR_POWER, SENSOR_READ)); // Alustetaan sensori
  initButton(onBtn, B2_BTN, CHANGE);                                                               // Alustetaan nappi
  attachInterrupt(digitalPinToInterrupt(onBtn.pin), switchPowerISR, onBtn.ISRState);               // Liitetään keskeytys
}

void loop() {
  if (timeToMeasure) {
    MeasurementTimes result;
    result.tbp = micros();                  // Otetaan talteen aika ennen virtaa

    tempSensor->powerOn();                  // Kytketään sensoriin virta
    result.start = micros();                // Otetaan talteen aika sen jälkeen kun virrat on kytketty

    resultArr[0] = tempSensor->measure();
    result.fmt = micros();                  // Otetaan talteen aika ensimmäisen mittauksen jälkeen

    // Loput mittaukset
    for (uint8_t i = 1; i < MEASUREMENTS_COUNT; i++) {
      resultArr[i] = tempSensor->measure();
      delayMicroseconds(MEASUREMENT_DELAY); 
    }
    result.end = micros();

    printResults(result);                   // Printataan tulokset

    timeToMeasure = false;
    tempSensor->powerOff();                 // Sensorista virrat pois
  }
}

// Funktio jolla tulostetaan tulokset sarjamonitoriin
void printResults(MeasurementTimes& result) {
  for (uint8_t i = 0; i < MEASUREMENTS_COUNT; i++) {
    p(resultArr[i]);
    if (i < 49) p(",");
  }
  pn(); pn();

  p("Aloitusaika:                     "); p(result.start);                     pn(" µs");
  p("Lopetusaika:                     "); p(result.end);                       pn(" µs");
  p("Mittausaika:                     "); p(result.end - result.start);        pn(" µs");
  p("Yhteen mittaukseen kulunut aika: "); p((result.end - result.start) / 50); pn(" µs");

  pn(); pn();
}

void initButton(Button& btn, const uint8_t pin, const uint8_t state) {
  btn.pin = pin;
  btn.ISRState = state;
  pinMode(btn.pin, INPUT);
}

void switchPowerISR() {
  timeToMeasure = onBtn.isPressed();
}
