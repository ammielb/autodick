# Code changelog

## Epaper_arduino
De code van `epaper_arduino.ino` is volledig herschreven om minimaal een demo te geven door de hardwaredata te sturen via I2C.

**Pin Definities en Constanten**

- Pin-definities herschreven met betekenisvolle namen.

- Knop- en schakelpinnen geconsolideerd in BUTTONS en SWITCHES arrays.

**GPS**
- GPS.(h/cpp) herschreven om de TinyGPS++ library te gebruiken.

**I2C**
- Code vereenvoudigt van `sendData`.

- `ReceiveData` is nog niet geïmplementeerd.

**SD Kaart Verwerking**
- Oude code initialiseerde en gebruikte de SD-kaart voor het loggen van hardware-staten en evenementen. De herschreven code implementeert momenteel geen SD-kaartlogging.

**Setup & Loop**
- `setup` herschreven met versimpelde pinMode-toewijzing
- `Loop` vereenvoudigt om de GPS bij te werken en knopindrukken te controleren om de hoorn te activeren.

**Algemeen**
- Alle functies comments gegeven.

## Epaper_main
De code van `epaper_main.ino` is grotendeels onaangepast gebleven, alleen veranderingen om de code werkende te krijgen zijn toegepast.

**RTC**
- De functie `getInput` aangepast om RTC te updaten met relevante GPS data als er een fix is.

**Display**
- De functie `showOverview` aangepast om de text met een kleiner lettertype text te weergeven. 

**Algemeen**
- Alle functies comments gegeven.