#include <IRremote.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>


#define LED_PIN  A0
#define LED_COUNT  25
#define IR_PIN 4

Servo myservo; 
int pos = 0;
Servo myservo2;
int pos2 = 0; 
Servo myservo3;
int pos3 = 0;
long timevar = 0;


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


uint8_t rValue = 0;
uint8_t gValue = 0;
uint8_t bValue = 0;

bool ledon=false;

void setup() {
  Serial.begin(9600);
  Serial.println("Enabling IRin");
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("Enabled IRin");
  myservo.attach(5);
  myservo2.attach(6);
  myservo3.attach(9);
  
  myservo.write(90);
  myservo2.write(90);
  myservo3.write(90);

  strip.begin();
  strip.show();
  strip.setBrightness(50);
}



void print_info()
{
  Serial.print(F("ROUGE : "));
  Serial.print(rValue);
  Serial.print(F("\tVERT : "));
  Serial.print(gValue);
  Serial.print(F("\tBLEU : "));
  Serial.println(bValue);
}
void reset_color()
{
  Serial.println("RESET");
  rValue = 0;
  gValue = 0;
  bValue = 0;
}
void set_led_color()
{
  int32_t color = strip.Color(rValue, gValue, bValue);
 strip.fill(color);
 strip.show();
}
int switch_color = 0;
int switch_color2 = 0;
void modify_green()
{
  rValue =  0;
  gValue = 255;
  bValue = 0;
}

void modify_red()
{
  rValue =  255;
  gValue = 0;
  bValue = 0;
}
void modify_blue()
{
  rValue =  0;
  gValue = 0;
  bValue = 255;
}







void loop() {


  if (IrReceiver.decode()) {

    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.command == 0x1C) {

    } else if (IrReceiver.decodedIRData.command == 0x46) {
      myservo.write(180);
      myservo2.write(0);
    } else if (IrReceiver.decodedIRData.command == 0x15) {
      myservo.write(0);
      myservo2.write(180);
    }
    else if (IrReceiver.decodedIRData.command == 0x43) {
      myservo.write(180);
      myservo2.write(180);
    } else if (IrReceiver.decodedIRData.command == 0x44) {
      myservo.write(0);
      myservo2.write(0);
    } else if (IrReceiver.decodedIRData.command == 0x16) {
      myservo3.write(150) ;
    }
    else if (IrReceiver.decodedIRData.command == 0x40) {
      myservo.write(90);
      myservo2.write(90);
      myservo3.write(90);
      reset_color();
    } else if (IrReceiver.decodedIRData.command == 0x44) {

    }
    else if (IrReceiver.decodedIRData.command == 0x42)
    {
      ledon = !ledon;
    }

    IrReceiver.resume();
  }

  if (millis() - timevar > 1200)
  {
    
    timevar = millis();
    IrReceiver.stop();
    if(ledon)
    {
      if (switch_color == 0)
      {
        modify_red();
        switch_color = 1;
      }
      else if (switch_color == 1)
      {
        modify_green();
        switch_color = 2;
      }
      else
      {
        modify_blue();
        switch_color = 0;
      }
      print_info();
      set_led_color();
    }
    else{
      
      strip.clear();
      strip.show();
    }
    IrReceiver.start();
  }
}
