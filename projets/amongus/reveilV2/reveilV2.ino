#include <Wire.h> //bibliothèque pour la communication avec les capteurs (I2C)
#include <PCF85063TP.h> //bibliothèque pour le capteur horloge
#include <rgb_lcd.h>//bibliothèque pour l'écran LCD
#include <Adafruit_NeoPixel.h>//bibliothèque pour les LED
#include <EEPROM.h>//bibliothèque pour sauvgarder des données 
#include <IRremote.h>

//constante definissant le numéro de la pin où sont branchées les LED
#define PIN_LED A3
//objet permettant de controller les LED
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(20, PIN_LED, NEO_GRB + NEO_KHZ800);
//objet permettant de controller l'écran LCD
rgb_lcd lcd;
//objet permettant de controller l'horloge RTC
PCD85063TP clock;

#define IR_PIN 5
#define BUZZER_PIN 6

//constantes indiquant les adresses où sont stokées les valeurs de l'alarme
#define EEPROMAHEURE 0
#define EEPROMAMINUTE 1
#define EEPROMAACTIVE 2

//variable contenant l'heure de l'alarme
unsigned char aheure = 9;
//variable contenant les minutes de l'alarme
unsigned char aminute = 0;
//variable booléen indiquant si l'alarme est active où non
bool alarmeon = false;

//variable contenant l'heure
unsigned char heure = 9;
//variable contenant la seconde actuelle
int seconde = 0;
//variable contenant la minute actuelle
int minute = 0;

bool ecranon = true;

bool ledson = false;

bool arcenciel = false;
long tempsarcenciel = 0;
#define FREQAEC 200

int rvalue = 0;
int vvalue = 0;
int bvalue = 0;

//variable contenant l'état du sequenceur gerant le menu et l'alarme
int mode = 0;

//index du chiffre changer dans l'heure 21:56  (0=2,1=1,2=5,3=6)
int haindex = 0;

char entree = 0;
long lastentree = 0;

long buzztemps = 0;



//fonction modifiant l'heure a laquelle l'alarme doit démarer
void setTempsAlarme(unsigned char pheure, unsigned char pminute) {
  if (pheure > 23) pheure = 0;
  if (pminute > 59) pminute = 0;
  aheure = pheure;
  aminute = pminute;
  EEPROM.write(EEPROMAHEURE, aheure);
  EEPROM.write(EEPROMAMINUTE, aminute);
}

//fonction modifiant l'heure
void setTemps(unsigned char pheure, unsigned char pminute) {
  if (pheure > 23) pheure = 0;
  if (pminute > 59) pminute = 0;
  heure = pheure;
  minute = pminute;
}

void setInput() {
  switch (IrReceiver.decodedIRData.command) {
    case 0x16: entree = '1'; break;
    case 0x19: entree = '2'; break;
    case 0xD: entree = '3'; break;
    case 0xC: entree = '4'; break;
    case 0x18: entree = '5'; break;
    case 0x5E: entree = '6'; break;
    case 0x08: entree = '7'; break;
    case 0x1C: entree = '8'; break;
    case 0x5A: entree = '9'; break;
    case 0x52: entree = '0'; break;
    case 0x42: entree = '*'; break;
    case 0x4A: entree = '#'; break;
    case 0x46: entree = 'z'; break;
    case 0x43: entree = 'd'; break;
    case 0x15: entree = 's'; break;
    case 0x44: entree = 'q'; break;
    case 0x40: entree = 'o'; break;
    default: entree = 0;
  }
}

//Focntion executée au démarage de la carte (mise sous tension)
void setup() {
  //Initialise la liaison série
  Serial.begin(9600);

  //Initialise les LED
  pixels.begin();
  pixels.show();

  //Initialise l'écran LCD
  lcd.begin(16, 2);
  //Affichage d'un message sur l'écran
  lcd.print("hello, world!");


  //initialisation du capteur de l'horloge
  clock.begin();
  //Réglage de l'horloge
  uint8_t ret = clock.calibratBySeconds(0, -0.000041);

  //on efface l'écran
  lcd.clear();

  //On récupère les données en mémoire et on initialise les variables
  setTempsAlarme(EEPROM.read(EEPROMAHEURE), EEPROM.read(EEPROMAMINUTE));
  alarmeon = EEPROM.read(EEPROMAACTIVE);


  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

}

//on déclenche ou on arette l'alarme
void setAlarme(bool on)
{
  if (on) //si on déclenche
  {
    mode = 10; //on passe en mode 10
    // ecrit un message sur l'ecran en le faisant clignoté
    lcd.blink();
    lcd.print("C'est l'heure!!!");
    ///////////////////CODE ICI////////////////////////////

    ///////////////////////////////////////////////////////
  }
  else
  {
    mode = 0; //on repasse en mode 0 (affichage de l'heure)
    lcd.noBlink(); //on arette le clignotement de l'écran
    ///////////////////CODE ICI////////////////////////////

    ///////////////////////////////////////////////////////
  }
}

void setPreAlarme(bool on)
{

  if (on) //si on déclenche
  {
    mode = 11; //on passe en mode 11
    // ecrit un message sur l'ecran en le faisant clignoté
    lcd.blink();
    lcd.print("Bientot l'heure!");
    ///////////////////CODE ICI////////////////////////////

    ///////////////////////////////////////////////////////
  }
  else
  {
    mode = 0; //on repasse en mode 0 (affichage de l'heure)
    lcd.noBlink(); //on arette le clignotement de l'écran
    ///////////////////CODE ICI////////////////////////////

    ///////////////////////////////////////////////////////
  }
}

//Fonction affichant l'heure sur l'écran LCD
void printTime()
{
  if (seconde != clock.second) //Si on a changer de seconde on met à jour l'affichage
  {
    //on met à jour la seconde actuelle
    seconde = clock.second;
    // on se déplace sur la colone 4 et la ligne 0 sur l'ecran LCD
    // répétition pour évité les erreurs d'affichage
    lcd.setCursor(4, 0);
    lcd.setCursor(4, 0);
    lcd.setCursor(4, 0);
    if (clock.hour < 10)lcd.print(0); //Si entre 0 et 9 on affiche un 0 avant
    lcd.print(clock.hour, DEC);//affichage de l'heure
    lcd.print(":");
    if (clock.minute < 10)lcd.print(0);
    lcd.print(clock.minute, DEC);
    lcd.print(":");
    if (clock.second < 10)lcd.print(0);
    lcd.print(clock.second, DEC);
  }
}

//Fonction affichant le menu de l'horloge sur l'écran LCD
void printConfTemps()
{
  lcd.setCursor(5, 0);
  lcd.print("Heure");

  lcd.setCursor(5, 1);
  if (heure < 10)lcd.print("0");
  lcd.print(heure, DEC);
  lcd.print(":");
  if (minute < 10)lcd.print(0);
  lcd.print(minute, DEC);
}

//Fonction affichant le menu de l'alarme sur l'écran LCD
void printAlarme()
{
  lcd.setCursor(4, 0);
  lcd.print("Alarme");
  lcd.setCursor(1, 1);
  if (alarmeon)
  {
    lcd.print("On");
  }
  else
  {
    lcd.print("Off");
  }
  lcd.setCursor(5, 1);

  if (aheure < 10)lcd.print("0");
  lcd.print(aheure, DEC);
  lcd.print(":");
  if (aminute < 10)lcd.print(0);
  lcd.print(aminute, DEC);
}

//Fonction contenant le sequenceur gérant le menu et l'alarme
//enc est le nombre de pas tourné sur le bouton rotatif (0 si immobile)
void menu() {
  //booleen indiquant s'il faut mettre à jour l'affichage
  //bouton appuyé ou bouton rotatif tournée
  bool change = false;
  if (entree == 'o') //Si on appuis sur le bouton Ok
  {
    change = true; //on mettera a jour l'affichage
    if (mode < 10) //Si on est en mode menu (mode inférieur à 10)
    {
      if (mode == 0)
      {
        heure = clock.hour;
        minute = clock.minute;

      }
      else if (mode == 1)
      {
        clock.stopClock();
        clock.fillByHMS(heure, minute, 00);
        clock.setTime();
        clock.startClock();
        lcd.noCursor();

      }
      mode++;
      if (mode > 3) {
        mode = 0;
        lcd.noCursor();
      }
    } else if (mode == 10 || mode == 11 ) //Si mode 10 (alarme allumé) on l'éteint
    {
      setPreAlarme(false);
      setAlarme(false);
    }
    Serial.print("mode ");
    Serial.println(mode);
  }


  if (entree != 0) //Si unn bouton à été appuyé, on mettera à jour l'affichage
  {
    change = true;
  }
  if (mode == 1) //Mode 3 on change l'heure
  {
    int val = 999;
    if (entree - '0' >= 0 && entree - '0' <= 9) {
      val = entree - '0';
    }
    if (haindex == 0 && val <= 2 )
    {
      setTemps(heure % 10 + 10 * val, minute);
      haindex ++;
    }
    else if (haindex == 1 && val <= 9)
    {
      setTemps((heure / 10) * 10 + val, minute);
      haindex ++;
    }
    else if (haindex == 2 && val <= 5)
    {
      setTemps(heure, minute % 10 + 10 * val);
      haindex ++;
    }
    else if (haindex == 3 && val <= 9)
    {
      setTemps(heure, (minute / 10) * 10 + val);
      haindex ++;
    }
    haindex = haindex % 4;
  }
  else if (mode == 2) //Mode 2 on change l'activation de l'alarme
  {
    if (entree == 'z' || entree == 's') {
      alarmeon = ! alarmeon;
      Serial.print("alarm ");
      Serial.println(alarmeon ? "on" : "off");
      EEPROM.write(EEPROMAACTIVE, alarmeon);
    }
  }
  else if (mode == 3) //Mode 3 on change l'heure de l'alarme
  {
    int val = 999;
    if (entree - '0' >= 0 && entree - '0' <= 9) {
      val = entree - '0';
    }
    if (haindex == 0 && val <= 2 )
    {
      setTempsAlarme(aheure % 10 + 10 * val, aminute);
      haindex ++;
    }
    else if (haindex == 1 && val <= 9)
    {
      setTempsAlarme((aheure / 10) * 10 + val, aminute);
      haindex ++;
    }
    else if (haindex == 2 && val <= 5)
    {
      setTempsAlarme(aheure, aminute % 10 + 10 * val);
      haindex ++;
    }
    else if (haindex == 3 && val <= 9)
    {
      setTempsAlarme(aheure, (aminute / 10) * 10 + val);
      haindex ++;
    }
    haindex = haindex % 4;
  }

  //Si il faut mettre à jour l'affichage, on efface l'écran, on eteint les pixels
  if (change)
  {
    Serial.println("Clear");
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.clear();
    pixels.clear();
    pixels.show();
  }

  //Gestion de l'affichage
  if (mode == 0) //Mode 0 affichage de l'heure
  {
    printTime();
  }
  else if (mode == 1 && change) {
    printConfTemps();
    lcd.setCursor(5 + (haindex < 2 ? haindex : haindex + 1), 1);
    lcd.cursor();
  }
  else if (2 <= mode  && mode <= 3 && change ) //Mode 1 à 3 affichage du menu de l'alarme
  {
    Serial.println("Menu Alarme");
    printAlarme();
    //on affiche de le chevron sur le paramètre à modifié
    if (mode == 2)
    {
      lcd.setCursor(0, 1);
      lcd.print(">");
    } else if (mode == 3)
    {
      lcd.setCursor(4, 1);
      lcd.print(">");
      lcd.setCursor(5 + (haindex < 2 ? haindex : haindex + 1), 1);
      lcd.cursor();
    }
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//Fonction étant exécutée en boucle
void loop() {

  if (IrReceiver.decode()) {
    if (millis() - lastentree > 500) {
      setInput();
      Serial.print("entree : ");
      Serial.println(entree);
    }
    lastentree = millis();
    IrReceiver.resume();
  }
  //on gère le menu et l'alarme
  menu();

  clock.getTime();
  //Si la minute acteulle a changée on vérifie l'alarme si on est pas en mode config
  if (minute != clock.minute && (mode < 1 || mode > 3))
  {
    minute = clock.minute;
    //Si l'heure et les minutes moins 5 minutes correspondent on déclenche la préalarme
    if (clock.minute == aminute - 5 && clock.hour == aheure && mode == 0 && alarmeon)
    {
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.clear();
      setPreAlarme(true);
    }
    //Si l'heure et les minutes correspondent on déclenche l'alarme
    if (clock.minute == aminute && clock.hour == aheure && mode == 0 && alarmeon)
    {
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.clear();
      setAlarme(true);
    }
  }


  if (mode == 0)
  {

    if (entree == '1') {
      ledson = true;
      arcenciel = false;
      rvalue = 200;
      vvalue = 200;
      bvalue = 200;
    }

    if (entree == '2') {
      ledson = true;
      arcenciel = false;
      rvalue = 200;
      vvalue = 0;
      bvalue = 0;
    }

    if (entree == '3') {
      ledson = true;
      arcenciel = false;
      rvalue = 170;
      vvalue = 50;
      bvalue = 0;
    }

    if (entree == '4') {
      ledson = true;
      arcenciel = false;
      rvalue = 100;
      vvalue = 200;
      bvalue = 50;
    }

    if (entree == '5') {
      ledson = true;
      arcenciel = false;
      rvalue = 0;
      vvalue = 200;
      bvalue = 10;
    }

    if (entree == '6') {
      ledson = true;
      arcenciel = false;
      rvalue = 0;
      vvalue = 170;
      bvalue = 70;
    }

    if (entree == '7') {
      ledson = true;
      arcenciel = false;
      rvalue = 0;
      vvalue = 0;
      bvalue = 200;
    }

    if (entree == '9') {
      ledson = true;
      arcenciel = false;
      rvalue = 170;
      vvalue = 0;
      bvalue = 70;
    }

    if (entree == '8') {
      ledson = true;
      arcenciel = false;
      rvalue = 100;
      vvalue = 0;
      bvalue = 170;
    }

    /////////////////////////////////////////////
    else if (entree == '0') {
      ledson = true;
      arcenciel = true;
    } else if (entree == '*') {
      ledson = !ledson;
    } else if (entree == '#') {
      ecranon = !ecranon;
      if (ecranon)
      {
        lcd.setRGB(0, 0, 0);
      }
      else {
        lcd.setRGB(255, 255, 255);
      }
    }



    if (ledson) {
      if (arcenciel && millis() - tempsarcenciel > FREQAEC )
      {
        tempsarcenciel = millis();
        for (int i = 0; i < pixels.numPixels(); i++) {
          pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + millis() / FREQAEC) & 255));
        }
        pixels.show();
      }
      else if (entree != 0)
      {
        pixels.fill(pixels.Color(rvalue, vvalue, bvalue));
        pixels.show();
      }
    }
    else
    {
      if (entree != 0)
      {
        pixels.clear();
        pixels.show();
      }
    }
  } else if (mode == 10)
  {
    if (millis() - buzztemps < 1000)
    {
      IrReceiver.stop();
      tone(BUZZER_PIN, 440);
    } else if (millis() - buzztemps < 2000)
    {
      noTone(BUZZER_PIN);
      IrReceiver.start();
    } else {
      buzztemps = millis();
    }
  }

  entree = 0;
  //on met en pause 1 milliseconde, ce qui économise de l'énergie
  delay(1);
}
