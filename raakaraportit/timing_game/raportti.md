Simppeli sovellus, jossa pääsi toteuttamaan hieman matematiikkaa ja aikavälimittausta, hieman 
tilastotiedettä myös sillä keskihajontaa laskettiin. Omana lisähaasteena yritän koodata mahdollisimman 
"kestävää" ja muistiturvallista C++:aa, sekä rakentaa myös hyvää sovellusarkkitehtuuria.

Olen alkanut käyttämään omissa C++ sovelluksissa pääasiassa etumerkittömänä kokonaislukuna
<cstdint> kirjastosta tulevaa uintn_t muuttujia, joka mahdollistaa sopimuksen, että 
alustasta riippumatta, esimerkiksi 32 bittininen etumerkitön kokonaisluku on aina 32 bittinen (Tai näin ainakin luvataan).

Logiikalta sitten käytin pääasiassa uint8_t ja uint32_t muuttujia. 8-bittisiä niihin, joissa ei säilytetä kovinkaan 
suuria lukuja, esim indeksiä tai klikkausten määrää. Casteja sitten käyttänyt tarpeen mukaan, sillä kääntäjä hoitaisi 
nämä kuitenkin, mutta yleensä näistä saataisiin varoituksia.

Pidin aikamuuttujat uint32_t muuttujissa, koska reipas 1000 tuntia todennäköisesti riittää että kyllästyy tähän peliin.
millis() palauttaa unsigned long, joka on saman kokoinen kuin uint32_t ainakin Metro M4 levyltä tullessa, sillä se on 32 bittinen, 
jossa unsigned long koko on 4 tavua. Arduinon omat constantin nappaan siihen tyyppin millä ne on määritelty, esimerkiksi käyn kurkkaamassa 
jos CHANGE on määritelty esimerkiksi #define 2, niin otan sen uint8_t muuttujaan.

Lopuksi vain muunsin nämä arvot floateiksi esitystä varten.

Pyrin käyttämään myös sovelluksissani smart pointtereita ja pitämään RAII mielessä.

Edit: huomasin myös seuraavassa tehtävässä että ns. throttle toiminnolle tuli 
tässäkin käyttöä, joten loin Throttled luokan ja kävin vielä refaktoroimassa tämän
tehtävän hyödyntämään sitä.


Printti:

1
2
3
4
5

Tavoiteaika oli 20.00s. Sait tulokseksi 21.28 s, eli virheesi oli 1.28 sekuntia.
Painallusten keskiarvo oli 5.32 sekuntia.
Painallusten keskihajonta oli 0.26 sekuntia.

Voit alustaa pelin B2 näppäimellä.