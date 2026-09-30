# 1. Luo uusi projekti (luo kansion ja saman-nimisen .ino-tiedoston)
arduino-cli sketch new OmaProjekti

# 2. Etsi ja asenna sensorikirjastoja
arduino-cli lib search vl53l1x
arduino-cli lib install "Adafruit VL53L1X"

# 3. Päivitä kirjastot tarvittaessa uusimpaan versioon
arduino-cli lib install "Adafruit VL53L1X" --upgrade

# Katso mikä portti (esim. /dev/ttyACM0) Metro M4:llä on käytössä
arduino-cli board list

# KAASU POHJAAN: Käännä ja lataa koodi suoraan laudalle yhdellä komennolla
arduino-cli compile --fqbn adafruit:samd:adafruit_metro_m4 -p /dev/ttyACM0 -u OmaProjekti.ino

# Cleani versio
arduino-cli compile --clean --fqbn adafruit:samd:adafruit_metro_m4 -p /dev/ttyACM0 -u smart_workstation_assistant.ino

# Jos haluat VAIN kääntää (testata virheet ilman laudan kytkemistä)
arduino-cli compile --fqbn adafruit:samd:adafruit_metro_m4 OmaProjekti.ino

# Vaihtoehto A: Arduino CLI:n oma työkalu
arduino-cli monitor -p /dev/ttyACM0 --config baudrate=115200

# Vaihtoehto B: Kevyt ja luotettava natiivityökalu (Suositus Linuxille)
tio -b 115200 /dev/ttyACM0

# 1. Luo väliaikainen kansio rakennustiedostoille
mkdir -p build

# 2. Generoi komentosolmu Clangd-LSP:lle (pakoitetaan build-kansioon)
arduino-cli compile --fqbn adafruit:samd:adafruit_metro_m4 --only-compilation-database --build-path ./build

# 3. Siirrä valmis JSON-tiedosto projektin juureen Neovimiä varten
mv build/compile_commands.json . && rm -rf build
