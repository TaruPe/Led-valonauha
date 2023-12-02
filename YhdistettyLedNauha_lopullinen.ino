// vaaditut kirjastot
#include <IRremote.h>
#include <IRremoteInt.h>
#include <FastLED.h>
#include <PololuLedStrip.h>

// määritetään käytettävät pinnit
#define IR_RECEIVE_PIN 8
#define LED_PIN 10
#define LED_COUNT 60

CRGB leds[LED_COUNT];
PololuLedStrip<10> ledStrip;
IRrecv IrReceive(IR_RECEIVE_PIN);

// kaukosäätimen numeroiden määritys
#define IR_BUTTON_0 22
#define IR_BUTTON_1 12
#define IR_BUTTON_2 24
#define IR_BUTTON_3 94
#define IR_BUTTON_4 8
#define IR_BUTTON_5 28
#define IR_BUTTON_6 90
#define IR_BUTTON_ON_OFF 69

bool isOn = false; // kertoo, onko laite päällä
int lightingMode = 0; // kertoo, missä moodissa valot ovat (oletusarvo 0)
unsigned long timerValue = 0;  // Laskuria varten on pidettävä muistissa aika
int colorState = 0; // Kertoo värien tilan
unsigned long time = millis(); // Laskuri napille 6

rgb_color colors[LED_COUNT];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, LED_COUNT);
  IrReceive.enableIRIn();
}

void loop(){
  checkIRCommands();
}

void checkIRCommands() {
  // Otetaan aivan aluksi talteen vanha tila, jotta voidaan katsoa, onko se muuttunut
  int oldLightingMode = lightingMode;
  bool lightingModeChanged = false;

  // Jos kaukosäädintä on painettu, luetaan painetun napin koodi
  if (IrReceive.decode()) {
    IrReceive.resume();
    lightingMode = IrReceive.decodedIRData.command;
    
    // Nollataan ajastin, eli otetaan talteen nykyinen ajanhetki
    timerValue = millis();

    // Tarkistetaan tässä, onko painettu virtanappia
    if (lightingMode == IR_BUTTON_ON_OFF) { // kaukosäätimen ON/OFF painike, jolla ohjataan LED-nauha päälle ja pois
      // Jos laite on jo päällä, sammutetaan se
      if (isOn) {
        fill_solid(leds, LED_COUNT, CRGB(0,0,0)); // asetetaan (pois päältä)
        FastLED.show();
        isOn = false;
        }
      // Jos laite ei ole päällä, käynnistetään se
      else {
        fill_solid(leds, LED_COUNT, CRGB(255,255,255)); // sytytä kaikki ledit
        FastLED.show();
        isOn = true;
      }
    }
  }

  // Muutetaan valojen moodia vain, jos laite on päällä
  if(isOn) {
    // Katsotaan ensin, onko painettu eri nappia kuin viimeksi
    if (lightingMode != oldLightingMode) {
      lightingModeChanged = true;
    }

    // Toimitaan eri moodien mukaisesti
    switch (lightingMode) {
      case IR_BUTTON_1: { // Käsittely painikkeelle 1, sytytetään punainen valo päälle
        // Tehdään tämä vain silloin, kun tila on muuttunut
        if (lightingModeChanged) {
          Serial.println("Painike 1 painettu");
          fill_solid(leds, LED_COUNT, CRGB(255, 0, 0)); // sytytä kaikki ledit punaiseksi
          FastLED.show();
        }
        break;
      }
      case IR_BUTTON_2: { //s ytytetään vihreä valo päälle
        // Tehdään tämä vain silloin, kun tila on muuttunut
        if (lightingModeChanged) {
          Serial.println("Painike 2 painettu");
          fill_solid(leds, LED_COUNT, CRGB(0, 255, 0)); // sytytä kaikki ledit vihreäksi
          FastLED.show();
        }
        break;
      }
      case IR_BUTTON_3: { // sytytetään sininen valo päälle
        // Tehdään tämä vain silloin, kun tila on muuttunut
        if (lightingModeChanged) {
          Serial.println("Painike 3 painettu");
          fill_solid(leds, LED_COUNT, CRGB(0, 0, 255)); // sytytä kaikki ledit siniseksi
          FastLED.show();
        }
        break;
      }
      case IR_BUTTON_4: { // sytytetään punainen vilkkuva valotoiminto päälle
        Serial.println("Painike 4 painettu");
        // Seuraava koodi suoritetaan aina, ei pelkästään tilan muuttuessa
        // Lasketaan, kauanko aikaa on kulunut siitä, kun tämä moodi laitettiin päälle
        unsigned long elapsedTime = millis() - timerValue;

        // Tarkistetaan, pitääkö valojen väriä muuttaa
        // Jos edellisestä kerrasta on kulunut 500 millisekuntia
        bool changeColor = false;
        if (elapsedTime >= 500) {
          changeColor = true; // Väri pitää muuttaa
          timerValue = millis();  // Nollataan ajastin
        }      

        // Vaihdetaan väri tarvittaessa
        if(changeColor) {
          // käytetään tässä lukuja tilan muuttamiseen 0 ja 1
          if (colorState == 0){
            fill_solid(leds, LED_COUNT, CRGB(255, 0, 0)); // asetetaan punainen väri
            FastLED.show();
            colorState = 1; // Muutetaan tila niin, että seuraavalla kerralla sammutetaan valot
          }
          else {
            fill_solid(leds, LED_COUNT, CRGB(0, 0, 0)); // asetetaan (pois päältä)
            FastLED.show();
            colorState = 0; // Muutetaan tila niin, että seuraavalla kerralla laitetaan valot päälle
          }
        }
        break;
      }
      case IR_BUTTON_5: { // sytytetään vihreä vilkkuva valotoiminto päälle
        Serial.println("Painike 5 painettu");
        // Seuraava koodi suoritetaan aina, ei pelkästään tilan muuttuessa
        // Lasketaan, kauanko aikaa on kulunut siitä, kun tämä moodi laitettiin päälle
        unsigned long elapsedTime = millis() - timerValue;

        // Tarkistetaan, pitääkö valojen väriä muuttaa
        // Jos edellisestä kerrasta on kulunut 500 millisekuntia
        bool changeColor = false;
        if (elapsedTime >= 500) {
          changeColor = true; // Väri pitää muuttaa
          timerValue = millis();  // Nollataan ajastin
        }      

        // Vaihdetaan väri tarvittaessa
        if(changeColor) {
          // käytetään tässä lukuja 0 ja 1 tilan tallentamiseen
          if (colorState == 0){
            fill_solid(leds, LED_COUNT, CRGB(0, 255, 0)); // asetetaan punainen väri
            FastLED.show();
            colorState = 1; // Muutetaan tila niin, että seuraavalla kerralla sammutetaan valot
          }
          else {
            fill_solid(leds, LED_COUNT, CRGB(0, 0, 0)); // asetetaan (pois päältä)
            FastLED.show();
            colorState = 0; // Muutetaan tila niin, että seuraavalla kerralla laitetaan valot päälle
          }
        }
        break;
      }
      
      case IR_BUTTON_6: { // sytytetään monivärinen liukuväri led nauhaan painettaessa ensimmäisen kerran nappia 6 saadaan aaltomainen valotoiminto ja painettaessa toisen kerran väri jää paikalleen moniväriseksi
        Serial.println("Painike 6 painettu");
      // Seuraava koodi suoritetaan aina, ei pelkästään tilan muuttuessa
      // Lasketaan, kauanko aikaa on kulunut siitä, kun tämä moodi laitettiin päälle
      unsigned long elapsedTime = millis() - timerValue;

      // Tarkistetaan, pitääkö valojen väriä muuttaa
      // Jos edellisestä kerrasta on kulunut 100 millisekuntia
      bool changeColor = false;
      if (elapsedTime >= 100) {
        changeColor = true; // Väri pitää muuttaa
        timerValue = millis();  // Nollataan ajastin
      }      

      // Vaihdetaan väri tarvittaessa
      if(changeColor) {
          time = millis()>> 2;
          for (uint16_t i = 0; i < LED_COUNT; i++)
          {
              byte x = time - 8*i;
              colors[i] = rgb_color(x, 255 - x, x);
          }
          // asetetaan värit led-nauhaan
          ledStrip.write(colors, LED_COUNT);
      }
        break;
    }
   
    case IR_BUTTON_0: { // pystytään ajastamaan nappien 1, 2, 3 ja 6 valot halutuksi ajaksi palamaan
      Serial.println("Painike 0 painettu");
  // Lasketaan, kauanko aikaa on kulunut siitä, kun tämä moodi laitettiin päälle
  unsigned long elapsedTime = millis() - timerValue;

  // Tarkistetaan, pitääkö valojen väriä muuttaa
  // Jos edellisestä kerrasta on kulunut 5000 millisekuntia
      colorState = 0;  
  if (elapsedTime >= 5000) { // odotetaan 5 sekuntia
      colorState = 1;
  
    //käytetään tässä lukuja 0 ja 1 tilan tallentamiseen
    if (colorState == 1) {
      fill_solid(leds, LED_COUNT, CRGB(0, 0, 0)); // asetetaan (pois päältä)
      FastLED.show();
    } 
  }
  break;
}

    }
  }
}  
