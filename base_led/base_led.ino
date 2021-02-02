#include <Adafruit_NeoPixel.h>

#define LED_PIN    A0
#define LED_COUNT  5
#define BUTTON_PIN 7

int buttonState = 0;         // variable for reading the pushbutton status

uint8_t rValue = 30;
uint8_t gValue = 30;
uint8_t bValue = 30;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
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

void modify_color()
{
  rValue = (rValue + 30);
  gValue = (gValue + 30);
  bValue = (bValue + 30);
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
    for(int i=0; i<strip.numPixels(); i++)
    {
      strip.setPixelColor(i, color);
      strip.show();
      delay(150);
    }
}

void loop()
{
    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState)
    {
      reset_color();
    }
    print_info();
    set_led_color();
    modify_color();
}
