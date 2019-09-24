float mAmpRec = 49;
float mAmpSleep = 3.4;
float mAmpCam = 700;
byte nBatPacks = 4;
float mAhPerBat = 12000.0; // assume 12Ah per battery pack; good batteries should be 14000

boolean settingsChanged = 0;

#define noSet 0
#define setRecDur 1
#define setRecSleep 2
#define setYearPage 3
#define setMonthPage 4
#define setDayPage 5
#define setHour 6
#define setMinute 7
#define setSecond 8
#define setFsamp 9
#define setBatPack 10
#define setMode 11
#define setStartHour 12
#define setStartMinute 13
#define setEndHour 14
#define setEndMinute 15

time_t autoStartTime;
 
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  display.print(":");
  printZero(digits);
  display.print(digits);
}

void printZero(int val){
  if(val<10) display.print('0');
}


void manualSettings(){
  boolean startRec = 0, startUp, startDown;

  autoStartTime = getTime();
    // make sure settings valid (if EEPROM corrupted or not set yet)
  
  if (rec_dur < 0 | rec_dur>100000) {
    rec_dur = 60;
  }
  if (rec_int<0 | rec_int>100000) {
    rec_int = 60;
  }
  if (startHour<0 | startHour>23) {
    startHour = 0;
  }
  if (startMinute<0 | startMinute>59) {
    startMinute = 0;
  }
  if (endHour<0 | endHour>23) {
    endHour = 0;
  }
  if (endMinute<0 | endMinute>59) {
    endMinute = 0;
  }
  if (recMode<0 | recMode>1) {
    recMode = 0;
  }
  
  while(startRec==0){
    static int curSetting = noSet;
    static int newYear, newMonth, newDay, newHour, newMinute, newSecond, oldYear, oldMonth, oldDay, oldHour, oldMinute, oldSecond;
    
    // Check for mode change
    boolean selectVal = digitalRead(enterButton);
    if(selectVal==0){
      curSetting += 1;
      while(digitalRead(enterButton)==0){ // wait until let go of button
        delay(10);
      }
      if((recMode==MODE_NORMAL & curSetting>11) | (recMode==MODE_DIEL & curSetting>15)) curSetting = 0;
   }

    cDisplay();

    t = RTCToUNIXTime(year, month, day, hour, minute, second);;

    if (t - autoStartTime > 600) startRec = 1; //autostart if no activity for 10 minutes
    switch (curSetting){
      case noSet:
        if (settingsChanged) {
          writeEEPROM();
          settingsChanged = 0;
          autoStartTime = getTime();  //reset autoStartTime
        }
        display.print("UP+DN->Rec"); 
        // Check for start recording
        startUp = digitalRead(upButton);
        startDown = digitalRead(downButton);
        if(startUp==0 & startDown==0) {
          cDisplay();
          writeEEPROM(); //save settings
          display.print("Starting..");
          display.display();
          delay(1500);
          startRec = 1;  //start recording
        }
        break;
      case setRecDur:
        rec_dur = updateVal(rec_dur, 1, 21600);
        display.print("Rec:");
        display.print(rec_dur);
        display.println("s");
        break;
      case setRecSleep:
        rec_int = updateVal(rec_int, 0, 3600 * 24);
        display.print("Slp:");
        display.print(rec_int);
        display.println("s");
        break;
      case setYearPage:
        oldYear = year;
        newYear = updateVal(oldYear,19, 99);
        if(oldYear!=newYear) setNewTime(hour, minute, second, day, month, newYear);
        display.print("Year:");
        getTime();
        display.print(year);
        break;
      case setMonthPage:
        oldMonth = month;
        newMonth = updateVal(oldMonth, 1, 12);
        if(oldMonth != newMonth) setNewTime(hour, minute, second, day, newMonth, year);
        display.print("Month:");
        getTime();
        display.print(month);
        break;
      case setDayPage:
        oldDay = day;
        newDay = updateVal(oldDay, 1, 31);
        if(oldDay!=newDay) setNewTime(hour, minute, second, newDay, month, year);
        display.print("Day:");
        getTime();
        display.print(day);
        break;
      case setHour:
        oldHour = hour;
        newHour = updateVal(oldHour, 0, 23);
        if(oldHour!=newHour) setNewTime(newHour, minute, second, day, month, year);
        display.print("Hour:");
        getTime();
        display.print(hour);
        break;
      case setMinute:
        oldMinute = minute;
        newMinute = updateVal(oldMinute, 0, 59);
        if(oldMinute!=newMinute) setNewTime(hour, newMinute, second, day, month, year);
        display.print("Minute:");
        getTime();
        display.print(minute);
        break;
      case setSecond:
        oldSecond = second;
        newSecond = updateVal(oldSecond, 0, 59);
        if(oldSecond!=newSecond) setNewTime(hour, minute, newSecond, day, month, year);
        display.print("Second:");
        getTime();
        display.print(second);
        break;
      case setFsamp:

        display.print("44.1 kHz");
        break;
      case setMode:
        display.print("Mode:");
        recMode = updateVal(recMode, 0, 1);
        if (recMode==MODE_NORMAL)  display.print("Norm");
        if (recMode==MODE_DIEL) display.print("Diel");
        break;
      case setBatPack:
        nBatPacks = updateVal(nBatPacks, 1, 8);
        display.print("Batt:");
        display.println(nBatPacks);
        break;
      case setStartHour:
        startHour = updateVal(startHour, 0, 23);
        display.print("Strt HH:");
        printZero(startHour);
        display.print(startHour);
        break;
      case setStartMinute:
        startMinute = updateVal(startMinute, 0, 59);
        display.print("Strt MM:");
        printZero(startMinute);
        display.print(startMinute);
        break;
      case setEndHour:
        endHour = updateVal(endHour, 0, 23);
        display.print("End HH:");
        printZero(endHour);
        display.print(endHour);
        break;
      case setEndMinute:
        endMinute = updateVal(endMinute, 0, 59);
        display.print("End MM:");
        printZero(endMinute);
        display.print(endMinute);
        break;
    }
    displaySettings();
    displayClock(getTime(), BOTTOM);
    display.display();
    delay(10);
  }
}
  
int updateVal(long curVal, long minVal, long maxVal){
  boolean upVal = digitalRead(upButton);
  boolean downVal = digitalRead(downButton);
  static int heldDown = 0;
  static int heldUp = 0;

  if(upVal==0){
    settingsChanged = 1;
    if (heldUp < 20) delay(200);
      curVal += 1;
      heldUp += 1;
    }
    else heldUp = 0;

    if (heldUp > 100) curVal += 4; //if held up for a while skip an additional 4
    if (heldUp > 200) curVal += 55; //if held up for a while skip an additional 4
    
    if(downVal==0){
      settingsChanged = 1;
      if(heldDown < 20) delay(200);
      if(curVal < 61) { // going down to 0, go back to slow mode
        heldDown = 0;
      }
        curVal -= 1;
        heldDown += 1;
    }
    else heldDown = 0;

    if(heldDown > 100) curVal -= 4;
    if(heldDown > 200) curVal -= 55;

    if (curVal < minVal) curVal = maxVal;
    if (curVal > maxVal) curVal = minVal;
    return curVal;
}

void cDisplay(){
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
}

void displaySettings(){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 18);

  display.print("Rec:");
  display.print(rec_dur);
  display.print("s ");
  if(recMode==MODE_DIEL){
    display.print(" Diel");
  }
  display.println();
  
  display.print("Sleep:");
  display.print(rec_int);
  display.print("s  ");

  display.print(" B:");
  display.println(nBatPacks);

  display.setTextSize(1);

  uint32_t totalRecSeconds = 0;

  float dielFraction = 1.0; //diel mode decreases time spent recording, increases time in sleep
  if(recMode==MODE_DIEL){
    float dielHours, dielMinutes;
    if(startHour>endHour){
      dielHours = (24 - startHour) + endHour; //  22 to 05 = 7
    }
    else{
      dielHours = endHour - startHour; //e.g. 10 to 12 = 2; 
    }
    if(startMinute > endMinute){
      dielMinutes = (60-startMinute) + endMinute;
    }
    else{
      dielMinutes = endMinute - startMinute;
    }
    dielMinutes += (dielHours * 60);
    dielFraction = dielMinutes / (24.0 * 60.0); // fraction of day recording in diel mode
  }

  float recDraw = mAmpRec;
  float recFraction = ((float) rec_dur * dielFraction) / (float) (rec_dur + rec_int);
  float sleepFraction = 1 - recFraction;
  float avgCurrentDraw = (recDraw * recFraction) + (mAmpSleep * sleepFraction);
  uint32_t powerSeconds = uint32_t (3600.0 * (nBatPacks * mAhPerBat / avgCurrentDraw));
}


void displayClock(time_t t, int loc){
  display.setTextSize(1);
  display.setCursor(0,loc);
  display.print(year);
  display.print('-');
  display.print(month);
  display.print('-');
  display.print(day);
  display.print("  ");
  printZero(hour);
  display.print(hour);
  printDigits(minute);
  printDigits(second);
}

void printTime(time_t t){
  Serial.print(year);
  Serial.print('-');
  Serial.print(month);
  Serial.print('-');
  Serial.print(day);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(':');
  Serial.print(minute);
  Serial.print(':');
  Serial.println(second);
}

void readEEPROM(){
//  rec_dur = readEEPROMlong(0);
//  rec_int = readEEPROMlong(4);
//  startHour = EEPROM.read(8);
//  startMinute = EEPROM.read(9);
//  endHour = EEPROM.read(10);
//  endMinute = EEPROM.read(11);
//  recMode = EEPROM.read(12);
//  isf = EEPROM.read(13);
//  byte newBatPacks = EEPROM.read(14);
//  if(newBatPacks>0)
//  {
//    nBatPacks = newBatPacks;
//  }
}

union {
  byte b[4];
  long lval;
}u;

long readEEPROMlong(int address){
//  u.b[0] = EEPROM.read(address);
//  u.b[1] = EEPROM.read(address + 1);
//  u.b[2] = EEPROM.read(address + 2);
//  u.b[3] = EEPROM.read(address + 3);
//  return u.lval;
}

void writeEEPROMlong(int address, long val){
//  u.lval = val;
//  EEPROM.write(address, u.b[0]);
//  EEPROM.write(address + 1, u.b[1]);
//  EEPROM.write(address + 2, u.b[2]);
//  EEPROM.write(address + 3, u.b[3]);
}

void writeEEPROM(){
//  writeEEPROMlong(0, rec_dur);  //long
//  writeEEPROMlong(4, rec_int);  //long
//  EEPROM.write(8, startHour); //byte
//  EEPROM.write(9, startMinute); //byte
//  EEPROM.write(10, endHour); //byte
//  EEPROM.write(11, endMinute); //byte
//  EEPROM.write(12, recMode); //byte
//  EEPROM.write(13, isf); //byte
//  EEPROM.write(14, nBatPacks); //byte
//  EEPROM.write(15, gainSetting); //byte
}
