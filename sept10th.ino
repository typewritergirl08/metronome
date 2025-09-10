#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define PIN_MP3_RX 2
#define PIN_MP3_TX 3

#define PIN_CLK 4
#define PIN_DT  5
#define PIN_SW  6

SoftwareSerial mp3Serial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

int vol = 20;
int lastCLK = HIGH;
bool muted = false;

void setup() {
  Serial.begin(9600);
  mp3Serial.begin(9600);

  Serial.println(F("Initializing DFPlayer..."));
  if (!player.begin(mp3Serial, true, false)) {
    Serial.println(F("DFPlayer init failed!"));
    while (1);
  }
  Serial.println(F("DFPlayer initialized"));

  player.volume(vol);
  Serial.print(F("Volume set to ")); Serial.println(vol);

  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_SW, INPUT_PULLUP); 
}

void loop() {

  int clkState = digitalRead(PIN_CLK);
  int dtState  = digitalRead(PIN_DT);

  if (clkState != lastCLK) {
    if (dtState != clkState) {
      vol++;
      if (vol > 30) vol = 30;
      Serial.print(F("Volume: ")); Serial.println(vol);
      if (!muted) player.volume(vol);
      Serial.println(F("Rotated clockwise"));
    } else {
      vol--;
      if (vol < 0) vol = 0;
      Serial.print(F("Volume: ")); Serial.println(vol);
      if (!muted) player.volume(vol);
      Serial.println(F("Rotated counter-clockwise"));
    }
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
      while (digitalRead(PIN_SW) == LOW) { delay(10); }
    }
  }

  player.play(1);
  delay(20000000000);
  player.stop();
  delay(20000000000);
}
