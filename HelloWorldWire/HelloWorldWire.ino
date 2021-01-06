// Test for minimum program size.
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <RH_ASK.h>
#define I2C_ADDRESS 0x3C

#define RED 3
#define GREEN 7

#define RST_PIN -1

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SSD1306AsciiWire oled;

double temp_amb;
double temp_obj;
bool object = false;
int count = 0;

const char *danger_msg = "1";
const char *cool_msg = "2";
const char *keep_alive = "0";

RH_ASK driver; // 0X3C+SA0 - 0x3C or 0x3D

void setup() {

  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000L);
  driver.init();
  mlx.begin();
  pinMode(RED, OUTPUT);     // Connect LASER
  pinMode(GREEN, OUTPUT);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.clear();
  oled.setFont(TimesNewRoman13_italic);
  oled.setCursor(1, 4);
  oled.print("Starting...");
  delay(1000);

}
void loop() {
  delay(1000);
  sentMsg(0);
  oled.clear();
  temp_amb = mlx.readAmbientTempC();
  temp_obj = mlx.readObjectTempC();
  delay(50);
  Serial.print("Room Temp = ");
  Serial.println(temp_amb);
  Serial.print("Object temp = ");
  Serial.println(temp_obj);
  object = false;

  if (temp_obj >= 32.00) {
    Serial.println("Object detected");
    object = true;
    oled.clear();
    temp_obj = mlx.readObjectTempC();
    oled.setCursor(1, 1);
    oled.setFont(TimesNewRoman13_italic);
    oled.print(" Temperature ");
    oled.setFont(fixed_bold10x15);
    oled.setCursor(1, 4);
    oled.print(temp_obj);
    oled.print((char)247);
    oled.print("C");
    delay(1500);
    
    if (temp_obj >= 34) {
      Serial.println("Danger");
      digitalWrite(GREEN, LOW);
      digitalWrite(RED, HIGH);
      oled.clear();
      oled.setCursor(1, 2);
      oled.setFont(fixed_bold10x15);
      oled.print("Access ");
      oled.setCursor(3, 4);
      oled.print("Denied!!");
      danger();
      sentMsg(1);
      delay(1000);
      
    }
      else if (temp_obj < 34 && object) {
        Serial.println("Cooool");
        digitalWrite(RED, LOW);
        oled.clear();
        oled.setCursor(1, 2);
        oled.setFont(fixed_bold10x15);
        oled.print("Access ");
        oled.setCursor(3, 4);
        oled.print("Granded :)");
        cool();
        sentMsg(2);
        delay(1000);
    }
  }
  else if (temp_obj < 32.00) {
    object = false;
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
    oled.setFont(fixed_bold10x15);
    oled.setContrast(256);
    oled.setCursor(1, 4);
    oled.print("Ready..");
  }
}

void danger(){
  bool state = 1;
  for(int i=0; i<=10; i++){
    digitalWrite(RED, state);
    state = !state;
    delay(100);
  }
}
void cool(){
  bool state = 1;
  for(int i=0; i<=10; i++){
    digitalWrite(GREEN, state);
    state = !state;
    delay(100);
  }
  digitalWrite(GREEN, LOW);
}

void sentMsg(int mode){
  Serial.println("Sending ...");
  oled.clear();
  oled.setFont(fixed_bold10x15);
  oled.setContrast(256);
  oled.setCursor(1, 4);
  oled.print("Sending..");
  switch (mode){
    case 0:
      driver.send((uint8_t *)keep_alive, strlen(keep_alive));
      driver.waitPacketSent();
      break;
      
    case 1:
      driver.send((uint8_t *)danger_msg, strlen(danger_msg));
      driver.waitPacketSent();
      break;
      
    case 2:
      driver.send((uint8_t *)cool_msg, strlen(cool_msg));
      driver.waitPacketSent();
      break;
      
    default:
      //nothing
      break;
  }
  Serial.println("Sent!!");
}
