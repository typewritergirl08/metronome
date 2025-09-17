//librariez
#include <U8g2lib.h> //lcd
#include <SoftwareSerial.h> //well duh
#include <DFRobotDFPlayerMini.h> //mp3

//df mp3 player thing 
#define PIN_MP3_RX 2
#define PIN_MP3_TX 3

//bpm dial
#define PIN_CLK 4
#define PIN_DT  5
#define PIN_SW  6

//lcd
#define LCD_BACKLIGHT 9

//ok i give up on commenting on everything
SoftwareSerial mp3Serial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

U8G2_ST7920_128X64_F_SW_SPI u8g2(
  U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8
); // i dont actually use 8 tho it's unwired

int vol = 20; //i dont use this yet though
int bpm = 60;
int lastCLK = HIGH;
bool muted = false;

void updateLCD() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  {
    char buf[20];
    sprintf(buf, "BPM:", bpm);
    u8g2.drawStr(0, 12, buf);
  }
  
//  if (muted)

  
  //  u8g2.drawStr(0, 12, "Muted");
    //else {
    //char buf[20];
    //sprintf(buf, "BPM: %d", bpm);
    //u8g2.drawStr(0, 12, buf);
  //}

  u8g2.sendBuffer();
}

void setup() {
  pinMode(LCD_BACKLIGHT, OUTPUT);
  analogWrite(LCD_BACKLIGHT, 70);

  Serial.begin(9600);
  mp3Serial.begin(9600);

  Serial.println(F("Initializing DFPlayer..."));
  if (!player.begin(mp3Serial, true, false)) {
    Serial.println(F("DFPlayer init failed!"));
    while (1);
  }
  Serial.println(F("DFPlayer initialized"));

 // player.volume(vol);
 // Serial.print(F("Volume: ")); Serial.println(vol);

  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_SW, INPUT_PULLUP);

  u8g2.begin();
  updateLCD();

  player.play(1);
  delay(1000);
}

void loop() {
  int clkState = digitalRead(PIN_CLK);
  int dtState  = digitalRead(PIN_DT);

  if (clkState != lastCLK) {
    if (dtState != clkState) {
      bpm++;
      if (bpm > 180) bpm = 180;
    } else {
      bpm--;
      if (bpm < 60) bpm = 60;
    }
    
    if (!muted) player.volume(vol);
    Serial.print(F("BPM: ")); Serial.println(bpm);
    updateLCD();
  }
  
  lastCLK = clkState;

  if (digitalRead(PIN_SW) == LOW) {
    delay(50);
    if (digitalRead(PIN_SW) == LOW) {
      muted = !muted;
      if (muted) {
        player.volume(0);
        Serial.println(F("muted"));
      } else {
        player.volume(vol);
        Serial.println(F("unmuted"));
      }
      updateLCD();

      while (digitalRead(PIN_SW) == LOW) {
        delay(10);
      }
    }
  }

unsigned long lastBeat = 0;
unsigned long interval = 60000 / bpm;
if (millis() - lastBeat >= interval) {
    lastBeat = millis();
    player.play(1);
}

}
