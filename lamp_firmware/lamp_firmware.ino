#include <Adafruit_NeoPixel.h>


#define RELAY_PIN D5
#define STRIP_PIN D6
#define LED_COUNT 10
#define TEMP_RESISTOR 100e3
#define TEMP_PRE_RESISTOR 220e3
#define TEMP_VOLTAGE 3.3


Adafruit_NeoPixel strip(LED_COUNT, STRIP_PIN, NEO_RGB + NEO_KHZ800);

void init_strip(uint32_t color, int wait){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  
  strip.begin();
  strip.show();
  strip.setBrightness(255);
}

void loop(){
  //digitalWrite(RELAY_PIN, 1);
  //init_strip(strip.Color(255, 255, 255), 20);
  //delay(5000);
  //digitalWrite(RELAY_PIN, 0);
  //init_strip(strip.Color(0, 0, 0), 20);
  int t = analogRead(A0);
  
  float U = t/1023.0f;
  float I = U/TEMP_RESISTOR;
  float R = (TEMP_VOLTAGE - U)/I - TEMP_PRE_RESISTOR;

  float T0 = 27.6 + 273.15;
  float R0 = 73920;
  float B = 4e3;
  float T = 1/(log(R/R0)/B + 1/T0) - 273.15;
  
  Serial.print("T: ");
  Serial.println(T);

  delay(500);
}
