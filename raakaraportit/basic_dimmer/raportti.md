Päädyin tässä käyttämään enemmän constexpr muuttujia, jotka määritellään jo käännösvaiheessa.
Tuntuu hyödylliseltä RAM-säästöltä, jos tätä käytetään muuttujiin, jotka eivät aidosti tule koskaan muuttumaan
ohjelman suorituksessa.

Selvästi helpompi mitä ensimmäinen tehtävä, mutta ihan mukava kun toteutti siten, että luokat ovat yleiskäyttöisempiä
ja arkkitehtuuri suht järkevä. Sain raapia päätä hetken, sillä yritin alussa toteuttaa siten, että liitän 
kaksi keskeytystä yhdelle napille, eli keskytys silloin kun FALLING ja RISING. Tämä ei tietenkään toiminut, sillä 
netin penkomisen jälkeen vain yksi keskeytys voidaan rekisteröidä yksittäiselle napille, joka selitti sen että 
jälkimmäisen alustuksen keskeytys vain toimi.

Light luokka oli mukava toteuttaa ja lambdoja on aina kiva päästä käyttämään. Lasken uuden kirkkausarvon kokonaisluvuilla, 
koska analogWrite ottaa myös valon kirkkauden vastaan kokonaislukuna. Tämä vain vaatii sitä että käytetään laskuissa 
isompia kokonaislukumuuttujien arvoja ja sitten tulos vain normalisoidaan analogWrite:lle sopivasti, eli 0-255 väliin.

Vastuiden osalta valoa vain kiinnostaa itsensä kirkkaus ja himmentimeltä tulee sitten arvo kuinka paljon 
valoa oikeasti halutaan himmentää tai kirkastaa.