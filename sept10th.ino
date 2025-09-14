#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define PIN_MP3_RX 2
#define PIN_MP3_TX 3

#define PIN_CLK 4
#define PIN_DT  5
#define PIN_SW  6

#define LCD_BACKLIGHT 9

SoftwareSerial mp3Serial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

U8G2_ST7920_128X64_F_SW_SPI u8g2(
  U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ U8X8_PIN_NONE
);

int vol = 20;
int lastCLK = HIGH;
bool muted = false;

void updateLCD() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  if (muted) {
    u8g2.drawStr(0, 12, "Muted");
  } else {
    char buf[20];
    sprintf(buf, "Volume: %d", vol);
    u8g2.drawStr(0, 12, buf);
  }

  u8g2.sendBuffer();
}

void setup() {
  pinMode(LCD_BACKLIGHT, OUTPUT);
  analogWrite(LCD_BACKLIGHT, 128);

  Serial.begin(9600);
  mp3Serial.begin(9600);

  Serial.println(F("Initializing DFPlayer..."));
  if (!player.begin(mp3Serial, true, false)) {
    Serial.println(F("DFPlayer init failed!"));
    while (1);
  }
  Serial.println(F("DFPlayer initialized"));

  player.volume(vol);
  Serial.print(F("Volume: ")); Serial.println(vol);

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
      vol++;
      if (vol > 30) vol = 30;
    } else {
      vol--;
      if (vol < 0) vol = 0;
    }
    if (!muted) player.volume(vol);
    Serial.print(F("Volume: ")); Serial.println(vol);
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
}
