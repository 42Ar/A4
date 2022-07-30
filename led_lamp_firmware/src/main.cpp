#include <Homie.h>
#include <Adafruit_NeoPixel.h>


#define FW_NAME                "lamp_control"
#define FW_VERSION             "1.0.0"
#define BRAND                  "A4_IOT"

#define RELAY_PIN D5
#define STRIP_PIN D6
#define LED_COUNT 85
#define TEMP_RESISTOR 100e3
#define TEMP_PRE_RESISTOR 220e3
#define TEMP_VOLTAGE 3.3
#define TEMP_SAMPLE_INTERVAL 10e3


Adafruit_NeoPixel strip(LED_COUNT, STRIP_PIN, NEO_RGB + NEO_KHZ800);
HomieNode node("lamp-0", "Lamp 1", "switch");
float temp = -100;
float effect_speed = 1;
int effect;
uint32_t color;


void init_strip(uint32_t color, int wait){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void set_strip_color(uint32_t color){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, color);
  }
  strip.show();
}

uint32_t ints_to_color(int red, int green, int blue){
  return green << 16 | red << 8 | blue;
}

bool string_to_color(const String &color, uint32 &res){
  int p1 = color.indexOf(',');
  int p2 = color.lastIndexOf(',');
  if(p1 == -1 || p1 == p2){
    return false;
  }
  uint32_t red = color.substring(0, p1).toInt() & 0xff;
  uint32_t green = color.substring(p1 + 1, p2).toInt() & 0xff;
  uint32_t blue = color.substring(p2 + 1).toInt() & 0xff;
  res = ints_to_color(red, green, blue);
  return true;
}

bool effect_handler(const HomieRange &range, const String &value){
  if(value == "off"){
    effect = 0;
  }else if(value == "on"){
    effect = 1;
  }else if(value == "chill"){
    effect = 2;
  }else if(value == "pride"){
    effect = 3;
  }else if(value == "rot"){
    effect = 4;
  }else if(value == "alert"){
    effect = 5;
  }else{
    return false;
  }
  node.setProperty("main").send(value);
  return true;
}

bool color_handler(const HomieRange &range, const String &value){
  bool success = string_to_color(value, color);
  if(success){
    node.setProperty("color").send(value);
  }
  return success;
}

bool main_handler(const HomieRange &range, const String &value){
  if(value == "true"){
    digitalWrite(RELAY_PIN, 0);
  }else if(value == "false"){
    digitalWrite(RELAY_PIN, 1);
  }else{
    return false;
  }
  node.setProperty("main").send(value);
  return true;
}

bool speed_handler(const HomieRange &range, const String &value){
  effect_speed = value.toFloat();
  node.setProperty("speed").send(String(effect_speed));
  return true;
}

void next_frame_chill(){
  return;
}

void next_frame_alert(){
  unsigned long t = millis();
  unsigned int millis_per_led = 10/effect_speed;
  for(int i = 0; i < strip.numPixels(); i++){
    int cell = (t/millis_per_led + i)/(LED_COUNT/6);
    strip.setPixelColor(i, ints_to_color(255*(cell&1), 0, 0));
  }
  strip.show();
}

void effect_next_frame(){
  switch(effect){
    case 0:
      set_strip_color(0);
      break;
    case 1:
      set_strip_color(color);
      break;
    case 2:
      next_frame_chill();
      break;
    case 5:
      next_frame_alert();
      break;
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println("\n\nSTARTUP FIRMWARE " FW_NAME " " FW_VERSION);

  Homie_setBrand(BRAND);
  Homie_setFirmware(FW_NAME, FW_VERSION);

  node.advertise("temp");
  node.advertise("reset-cause");
  node.advertise("effect").settable(effect_handler);
  node.advertise("color").settable(color_handler);
  node.advertise("speed").settable(speed_handler);
  node.advertise("main").setDatatype("boolean").settable(main_handler);
  Homie.disableResetTrigger();
  Homie.setup();

  color = ints_to_color(255, 255, 255);
  effect = 0;
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, 0);
  strip.begin();
  strip.setBrightness(255);
  strip.show();
}

void loop(){
  Homie.loop();
  static bool first = true;
  if(first){
    rst_info *rtc_info = system_get_rst_info();
    node.setProperty("reset-cause").send(String(rtc_info->reason));
    first = false;
  }
  static unsigned long last = 0;
  unsigned long cur = millis();
  if(cur - last > TEMP_SAMPLE_INTERVAL){
    last = cur;
    int t = analogRead(A0);
    float U = t/1023.0f;
    float I = U/TEMP_RESISTOR;
    float R = (TEMP_VOLTAGE - U)/I - TEMP_PRE_RESISTOR;
    float T0 = 27.6 + 273.15;
    float R0 = 73920;
    float B = 4e3;
    temp = 1/(log(R/R0)/B + 1/T0) - 273.15;
    node.setProperty("temp").send(String(temp));

    rst_info *rtc_info = system_get_rst_info();
    node.setProperty("reset-cause").send(String(rtc_info->reason));
  }
  effect_next_frame();
}