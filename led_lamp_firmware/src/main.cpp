#include <Homie.h>
#include <Adafruit_NeoPixel.h>


#define FW_NAME    "main-lamp-firmware"
#define FW_VERSION "1.0.0"
#define BRAND      "A4-IOT"

#define RELAY_PIN D5
#define STRIP_PIN D6
#define LED_COUNT 85
#define TEMP_U0 3.3
#define TEMP_R1 10e3
#define TEMP_R2 220e3
#define TEMP_R3 100e3
#define TEMP_RT 10e3
#define TEMP_SAMPLE_INTERVAL 20e3
#define PERF_REPORT_INTERVAL 20e3

#define DISABLE_LOGGING

Adafruit_NeoPixel strip(LED_COUNT, STRIP_PIN, NEO_RGB + NEO_KHZ800);
HomieNode node("lamp", "Lamp", "lamp");
float temp = -100;
float effect_speed = 1;
int effect = 0;
unsigned static int target_fps = 60;
uint32_t color;
bool main_on = true;


void set_main(bool new_state);

void set_strip_color(uint32_t color){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, color);
  }
  strip.show();
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
    default: return "???";
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
  if(effect == 0 || effect == 4 || effect == 6){
    set_strip_color(0);
  }
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

bool cmd_handler(const HomieRange &range, const String &value){
  if(value == "reset"){
      Homie.reset();
  }else{
    return false;
  }
  node.setProperty("cmd").send(value);
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
  }
}

float calc_temp(){
  float U = analogRead(A0)/1023.0f;
  float I1 = U/TEMP_R3;
  float U1 = I1*(TEMP_R2 + TEMP_R3);
  float I0 = (TEMP_U0 - U1)/TEMP_R1;
  if(I0 - I1 <= 0){
    return -100;
  }
  float R = U1/(I0 - I1);
  float T0 = 25 + 273.15;
  float B = 3950;
  float v = log(R/TEMP_RT)/B + 1/T0;
  if(v <= 0){
    return -100;
  }
  float t = 1/v - 273.15;
  if(t < -100){
    return -100;
  }
  return t;
}

void homie_loop(){
  bool ready = Homie.isConnected() && Homie.isConfigured();
  static unsigned long last_temp_report = 0;
  unsigned long cur = millis();
  if(ready && cur - last_temp_report > TEMP_SAMPLE_INTERVAL){
    last_temp_report = cur;
    float temp = calc_temp();
    node.setProperty("temp").send(String(temp));
    rst_info *rtc_info = system_get_rst_info();
    node.setProperty("reset-cause").send(String(rtc_info->reason));
  }
  static unsigned long last_frame = 0, skips = 0, frames = 0;
  cur = millis();
  if(cur - last_frame > 1000/target_fps){
    last_frame = cur;
    effect_next_frame();
    frames += 1;
  }else{
    skips += 1;
  }
  static unsigned long last_perf_report = 0;
  cur = millis();
  
  if(ready && cur - last_perf_report > PERF_REPORT_INTERVAL){
    node.setProperty("load").send(String(frames*100.0f/(frames + skips)));
    node.setProperty("fps").send(String(frames*1000.0f/(cur - last_perf_report)));
    last_perf_report = cur;
    frames = 0;
    skips = 0;
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
  Homie.disableResetTrigger();
#ifdef DISABLE_LOGGING
  Homie.disableLogging();
#endif
  Homie.setLoopFunction(homie_loop);

  node.advertise("temp").setDatatype("float").setUnit("°C");
  node.advertise("reset-cause").setDatatype("integer");
  node.advertise("sleep-mode").setDatatype("integer");
  node.advertise("effect").settable(effect_handler);
  node.advertise("color").settable(color_handler);
  node.advertise("cmd").settable(cmd_handler);
  node.advertise("speed").setDatatype("float").settable(speed_handler);
  node.advertise("main").setDatatype("boolean").settable(main_handler);
  Homie.setup();
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
}

void loop(){
  Homie.loop();
  
  // this is a workaround, since disableResetTrigger disables hardware and software reset
  if(HomieInternals::Interface::get().reset.resetFlag){
    HomieInternals::Interface::get().getConfig().erase();
    HomieInternals::Interface::get().getConfig().setHomieBootModeOnNextBoot(HomieBootMode::CONFIGURATION);
    ESP.restart();
  }
}