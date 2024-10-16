#include <Homie.h>
#include <Adafruit_NeoPixel.h>


#define FW_NAME    "main-lamp-firmware"
#define FW_VERSION "1.0.0"
#define BRAND      "A4-IOT"

//=== old board
// #define RELAY_PIN D5
// #define LED_PIN D6

//=== new board
#define RELAY_PIN 5u
#define LED_PIN 4u
#define RESET_BTN_PIN 13u
#define DISABLE_LOGGING


#define LED_COUNT 85
#define TEMP_U0 3.3
#define TEMP_R1 10e3
#define TEMP_R2 220e3
#define TEMP_R3 100e3
#define TEMP_RT 10e3
#define TEMP_SAMPLE_INTERVAL 20e3
#define PERF_REPORT_INTERVAL 20e3


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
HomieNode node("lamp", "Lamp", "lamp");
float temp = -100;
float effect_speed = 1;
int effect = 0;
unsigned static int target_fps = 60;
uint32_t color;
bool main_on = true;

constexpr int bounce_obj = 5;
long last_bounce = 0;
static float bounce_x[bounce_obj];
static float bounce_v[bounce_obj];
static float bounce_charge[bounce_obj];


void set_main(bool new_state);

void set_strip_color(uint32_t color){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, color);
  }
  strip.show();
}

uint32_t ints_to_color(int red, int green, int blue){
  return green << 16 | red << 8 | blue;
}

String effect_to_name(int id){
  switch(id){
    case 0: return "off";
    case 1: return "on";
    case 2: return "chill";
    case 3: return "pride";
    case 4: return "fire";
    case 5: return "alert";
    case 6: return "matrix";
    case 7: return "bounce";
    case 8: return "xmas";
    case 9: return "displayframes";
    default: return "???";
  }
}

void init_effect(){
  if(effect == 0 || effect == 4 || effect == 6){
    set_strip_color(0);
  }else if(effect == 7){
    for(int i = 0; i < bounce_obj; i++){
      bounce_x[i] = float(random(10000))/10000;
      bounce_charge[i] = random(2)*2 - 1;
      bounce_v[i] = 0;
    }
    set_strip_color(ints_to_color(255, 255, 255));
    last_bounce = millis();
  }else if(effect == 8){
    for(int i = 0; i < strip.numPixels(); i++){
      int cell = double(i)/(strip.numPixels() - 1)*18;
      strip.setPixelColor(i, ints_to_color(255*(cell&1), 255*(!(cell&1)), 0));
    }
    strip.show();
  }
}

void set_effect(int new_effect){
  if(effect == new_effect){
    return;
  }
  effect = new_effect;
  if(effect != 0){
    set_main(false);
  }
  init_effect();
  node.setProperty("effect").send(effect_to_name(effect));
}

void set_main(bool new_main_on){
  if(main_on == new_main_on){
    return;
  }
  main_on = new_main_on;
  if(main_on){
    set_effect(0);
  }
  digitalWrite(RELAY_PIN, !main_on);
  node.setProperty("main").send(main_on?"true":"false");
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
  int new_effect;
  if(value == "off"){
    new_effect = 0;
  }else if(value == "on"){
    new_effect = 1;
  }else if(value == "chill"){
    new_effect = 2;
  }else if(value == "pride"){
    new_effect = 3;
  }else if(value == "fire"){
    new_effect = 4;
  }else if(value == "alert"){
    new_effect = 5;
  }else if(value == "matrix"){
    new_effect = 6;
  }else if(value == "bounce"){
    new_effect = 7;
  }else if(value == "xmas"){
    new_effect = 8;
  }else{
    return false;
  }
  set_effect(new_effect);
  return true;
}

bool main_handler(const HomieRange &range, const String &value){
  bool new_main_on;
  if(value == "true"){
    new_main_on = true;
  }else if(value == "false"){
    new_main_on = false;
  }else{
    return false;
  }
  set_main(new_main_on);
  return true;
}

bool frame_handler(const HomieRange &range, const String &value){
  for(int i = 0; i < strip.numPixels(); i++){
    int off = i*2*3;
    if(off + 2*3 >= value.length()){
      return false;
    }
    uint32_t color = 0;
    for(int j = 0; j < 6; j++){
      color <<= 4;
      char c = value[off + j];
      if(c >= 'a' && c <= 'f'){
        color |= c - 'a' + 9;
      }else if(c >= '0' && c <= '9'){
        color |= c - '0';
      }else{
        return false;
      }
    }
    strip.setPixelColor(i, color);
  }
  return true;
}

bool color_handler(const HomieRange &range, const String &value){
  bool success = string_to_color(value, color);
  if(success){
    node.setProperty("color").send(value);
  }
  return success;
}

bool speed_handler(const HomieRange &range, const String &value){
  effect_speed = value.toFloat();
  node.setProperty("speed").send(String(effect_speed));
  return true;
}

void next_frame_bounce(){
  unsigned long cur = millis();
  int dt = effect_speed*(cur - last_bounce)*0.1;
  last_bounce = cur;
  for(int i = 0; i < bounce_obj; i++){
    float F = 0;
    for(int j = 0; j < bounce_obj; j++){
      if(i != j){
        float d = bounce_x[j] - bounce_x[i];
        if(d > 0.5){
          d = d - 1;
        }else if(d < -0.5){
          d = d + 1;
        }
        F -= 0.0005*bounce_charge[i]*bounce_charge[j]*d;
      }
    }
    bounce_v[i] += F*dt;
  }
  for(int i = 0; i < bounce_obj; i++){
    bounce_x[i] += bounce_v[i]*dt;
    if(bounce_x[i] >= 1){
      bounce_x[i] -= 1;
    }else if(bounce_x[i] < 0){
      bounce_x[i] += 1;
    }
  }
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, ints_to_color(100, 100, 100));
  }
  for(int i = 0; i < bounce_obj; i++){
    int led = int(round(LED_COUNT*bounce_x[i]))%LED_COUNT;
    if(bounce_charge[i] > 0){
      strip.setPixelColor(led, ints_to_color(255, 0, 0));
    }else{
      strip.setPixelColor(led, ints_to_color(0, 0, 255));
    }
  }
  strip.show();
}

void next_frame_chill(){
  int f = int(20000*effect_speed);  // 20 seconds for a full cycle
  unsigned int p = millis()%f;  //value in [0, f-1]
  float x = 3*float(p)/f;  // value in [0, 3)
  int i = int(x);  // 0, 1 or 2
  int v = 255*(x - i);  // value in [0, 255)
  if(i == 0){
    set_strip_color(ints_to_color(v, 0, 255 - v));
  }else if (i == 1){
    set_strip_color(ints_to_color(255 - v, v, 0));
  }else if (i == 2){
    set_strip_color(ints_to_color(0, 255 - v, v));
  }
}

void next_frame_alert(){
  unsigned long t = millis();
  float led_per_millis = effect_speed*0.1f;
  for(int i = 0; i < strip.numPixels(); i++){
    int cell = (t*led_per_millis + i)/(LED_COUNT/6);
    strip.setPixelColor(i, ints_to_color(255*(cell&1), 0, 0));
  }
  strip.show();
}

void next_frame_matrix(){
  static unsigned long last = 0;
  if(millis() - last > 1000/effect_speed){
    last = millis();
    int led = random(LED_COUNT);
    if(strip.getPixelColor(led) == 0){
      strip.setPixelColor(led, ints_to_color(0, 255, 0));
    }else{
      strip.setPixelColor(led, 0);
    }
    strip.show();
  }
}

void next_frame_fire(){
  static unsigned long last = 0;
  if(millis() - last > 1000/effect_speed){
    last = millis();
    int led = random(LED_COUNT);
    strip.setPixelColor(led, ints_to_color(255, random(15, 40), 0));
    strip.show();
  }
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
    case 4:
      next_frame_fire();
      break;
    case 5:
      next_frame_alert();
      break;
    case 6:
      next_frame_matrix();
      break;
    case 7:
      next_frame_bounce();
      break;
  }
}

void setup(){
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, 0);
  
  color = ints_to_color(255, 255, 255);
  strip.begin();
  strip.setBrightness(255);
  set_strip_color(0);

#ifndef DISABLE_LOGGING
  Serial.begin(115200);
#endif
  Homie_setBrand(BRAND);
  Homie_setFirmware(FW_NAME, FW_VERSION);
#ifdef RESET_BTN_PIN
  Homie.setResetTrigger(RESET_BTN_PIN, LOW, 2000);
#else
  Homie.disableResetTrigger();
#endif
#ifdef DISABLE_LOGGING
  Homie.disableLogging();
#endif

  node.advertise("temp").setDatatype("float").setUnit("°C");
  node.advertise("reset-cause").setDatatype("integer");
  node.advertise("sleep-mode").setDatatype("integer");
  node.advertise("effect").settable(effect_handler);
  node.advertise("color").settable(color_handler);
  node.advertise("speed").setDatatype("float").settable(speed_handler);
  node.advertise("main").setDatatype("boolean").settable(main_handler);
  node.advertise("frame").settable(frame_handler);
  Homie.setup();
}

void loop(){
  Homie.loop();
  
  static unsigned long last_frame = 0, skips = 0, frames = 0;
  unsigned long cur = millis();
  if(cur - last_frame > 1000/target_fps){
    last_frame = cur;
    effect_next_frame();
    frames += 1;
  }else{
    skips += 1;
  }
  static unsigned long last_perf_report = 0;
  cur = millis();

  bool ready = Homie.isConnected() && Homie.isConfigured();
  if(ready && cur - last_perf_report > PERF_REPORT_INTERVAL){
    node.setProperty("load").send(String(frames*100.0f/(frames + skips)));
    node.setProperty("fps").send(String(frames*1000.0f/(cur - last_perf_report)));
    last_perf_report = cur;
    frames = 0;
    skips = 0;
  }
}