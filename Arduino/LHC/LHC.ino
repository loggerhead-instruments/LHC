//
// LHC Cyclops Pi c2019
// David Mann
//

// Program with Native USB Port

// Pin Mapping
// Function   ATSAM    Pi
// requestOff 8 input  37 (GPIO 25) output
// turnOff    9 output 33 (GPIO 23) input
// piStatus   4 input  26 (GPIO 11) output


// 
// To Do:
// - add button labels to bottom: UP DN EN
// - cycle Pi recording based on duty cycle settings
// - sleep ATSAM
// - calculate power consumption
// - save settings to EEPROM

#include <Wire.h>
#include <RTCZero.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define BOTTOM 55

RTCZero rtc;

#define ledGreen 5
#define ledRed 2
#define piPower A0 
#define enableAudio A5
#define piRequestOff 8
#define piTurnOff 9
#define piStatus 4
#define piReset 31
#define batVoltage A4
#define upButton A3 // PA04
#define downButton A2 // PB09
#define enterButton A1 // PB08
// LEDs are on when pulled low
#define ledOn LOW
#define ledOff HIGH

byte hour, minute, second, day, month, year;
int mode = 0;  // 0=stopped, 1=recording, 2=playing
time_t startTime;
time_t stopTime;
time_t t;

byte startHour, startMinute, endHour, endMinute; //used in Diel mode
long dielRecSeconds;

#define MODE_NORMAL 0
#define MODE_DIEL 1
int recMode = MODE_NORMAL;
long rec_dur = 10;
long rec_int = 30;

int snooze_hour;
int snooze_minute;
int snooze_second;

void setup() {
  SerialUSB.begin(115200);
  Serial1.begin(115200);  // connected to 0(RX), 1(TX)

  rtc.begin();
  hour =10;
  minute = 0;
  second = 0;
  day = 24;
  month = 9;
  year = 19;
  
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(piPower, OUTPUT);
  pinMode(enableAudio, OUTPUT);
  pinMode(piRequestOff, INPUT);  // Pi turns high when done
  pinMode(piTurnOff, OUTPUT);  // Tells Pi to do software shutdown
  pinMode(piStatus, INPUT);  // 1 when recording video
  pinMode(piReset, INPUT);
  pinMode(batVoltage, INPUT);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(enterButton, INPUT_PULLUP);
  
  digitalWrite(ledGreen, ledOn);
  digitalWrite(ledRed, ledOff);
  digitalWrite(enableAudio, HIGH);
  digitalWrite(piPower, HIGH);
  digitalWrite(piTurnOff, LOW);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //initialize display
  delay(150);
  cDisplay();

  delay(5000);
  rtc.setTime(hour, minute, second);
  rtc.setDate(day, month, year);
  manualSettings();
  
  while(1){
    Serial1.println("Lat 26 Lon -82");
    SerialUSB.println("Lat 26 Lon -82");
    digitalWrite(ledGreen, ledOn);
    SerialUSB.print("V:"); SerialUSB.println(readVoltage());
    delay(2000);
    digitalWrite(ledGreen, ledOff);
    delay(2000);

//    rtc.setAlarmTime(10, 00, 10); // alarm at 10:00:10
//    rtc.enableAlarm(rtc.MATCH_HHMMSS);
//    rtc.attachInterrupt(ISR);
//    rtc.standbyMode();
  }

    // wait for pi to start recording
    while(digitalRead(piStatus)==0){
      delay(500);
      digitalWrite(ledGreen, ledOn);
      delay(500);
      digitalWrite(ledGreen, ledOff);
    }
}

void loop() {
  delay(2000);
  SerialUSB.print("volts: ");
  SerialUSB.println(readVoltage());
  SerialUSB.print("status: ");
  int recording = digitalRead(piStatus);
  SerialUSB.println(digitalRead(piStatus));
  SerialUSB.print("requestOff: ");
  int piDone = digitalRead(piRequestOff);
  SerialUSB.println(piDone);

  if(recording==0 & piDone==1){
    digitalWrite(piTurnOff, HIGH);  // tell Pi to do software shutdown
    while(digitalRead(piRequestOff)){
      digitalWrite(ledRed, ledOn);
      delay(100);
      digitalWrite(ledRed, ledOff);
      delay(400);
    }
    SerialUSB.println("Sleep");
    SerialUSB.println();
    digitalWrite(piPower, LOW); // power down Pi
    digitalWrite(enableAudio, LOW);  // power down audio
    delay(120000); // wait 2 minutes
    digitalWrite(piPower, HIGH); // power up Pi
    digitalWrite(enableAudio, HIGH); // power up audio

    // wait for pi to start recording
    while(digitalRead(piStatus)==0){
      delay(500);
      digitalWrite(ledGreen, ledOn);
      delay(500);
      digitalWrite(ledGreen, ledOff);
    }

  }
}

float readVoltage(){
  float vDivider = 0.5;
  float vReg = 3.3;
  float voltage = (float) analogRead(batVoltage) * vReg / (vDivider * 1024.0);
  return voltage;
}

void ISR(){
  // clock alarm wakeup
  digitalWrite(ledGreen, ledOn);
}
