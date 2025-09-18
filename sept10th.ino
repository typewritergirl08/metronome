//librariez
#include <U8g2lib.h> //lcd
#include <SoftwareSerial.h> //well duh
#include <DFRobotDFPlayerMini.h> //mp3

//df mp3 player thing pins
//#define PIN_MP3_RX 2
//#define PIN_MP3_TX 3
const short rx = 2
const short tx = 3

//bpm dial pins
#define PIN_CLK 4
#define PIN_DT  5
#define PIN_SW  6

//lcd pins
#define LCD_BACKLIGHT 9

//ok i give up on commenting on everything
//SoftwareSerial mp3Serial(PIN_MP3_RX, PIN_MP3_TX);
//DFRobotDFPlayerMini player;

MD_YX5300 mp3(rx, tx);

U8G2_ST7920_128X64_F_SW_SPI u8g2(
  U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8
); // i dont actually use 8 tho it's unwired but easier if assigned

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
  
  u8g2.sendBuffer();
}

unsigned long lastBeat = 0;
unsigned long interval = 60000 / bpm; //this math is right

void setup() {
  pinMode(LCD_BACKLIGHT, OUTPUT);
  analogWrite(LCD_BACKLIGHT, 70);

  Serial.begin(9600);
  mp3.begin();

  //Serial.println(F("Initializing DFPlayer..."));
  //if (!player.begin(mp3Serial, true, false)) {
   // Serial.println(F("DFPlayer init failed!"));
    //while (1);
  //}
  //Serial.println(F("DFPlayer initialized")); //idk this was example code

  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_SW, INPUT_PULLUP);

  u8g2.begin();
  updateLCD();

}

void loop() { //is there a way to have a "loop2"? ie something that runs parallel to loop, and also keeps iteratively calling itself

  //put this in its own function

  
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
    
    interval = 60000 / bpm;
  }

  //------------------------------------------------------------------------------
 
      updateLCD();

  //---------------------------------------------------------------------------
      
  //this will need to run independly of the firstloop
  if (millis() - lastBeat >= interval) {
    lastBeat = millis();
    mp3.playTrack(1); 
  }
  
}
