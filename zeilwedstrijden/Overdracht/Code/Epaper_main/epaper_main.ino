/**
 * @file epaper_main.ino
 * @brief Meant to be run on the Epaper in the Auto Dick 3.
 * 
 * @details This code receives the encoded hardware state from the arduino over an I2C connection.
 * Tt uses this state data to determen the of the application and displays it on the Epaper screen.
 * 
 * This is legacy code in need of refactoring (or better yet, complete re-design). 
 */

#include <M5EPD.h>
#include "Rtc.hpp"

#define WIDTH 960
#define HEIGHT 540
#define GLOBAL_UPDATE_MODE UPDATE_MODE_NONE
#define M5Touch false
#define M5SD false
#define M5Serial true
#define M5Battery false
#define M5I2C true

M5EPD_Canvas canvas(&M5.EPD);
Rtc RTC = Rtc();

rtc_time_t lastTime;
rtc_time_t GPSTime;
String lastLines[5];
String lastNextFlagString;
String lastCurrentString;
int inputData[9];
int lastInputData[9];
bool sendHorn = false;
int encoderChange;
bool SDCard = false;
bool lastSDCard = false;
int soundFragment = 0;
bool sendSoundFragment = false;
bool fragmentPlayed[3] = {false, false, false};
int fragmentsPlayed = 0;
int currentSoundFragment = 1;
int totalSoundFragments = 30;
int lastStage = -1;
int lastAborts[25];
bool aborted = false;
bool logMade = false;
rtc_time_t sendTime;
int sendSetting = 0;
int sendFlag = 0;

rtc_time_t startTime;
rtc_time_t lastStartTime;
rtc_time_t lastFlagTiming;
bool started = false;
bool repeated = false;
bool lastRepeated = false;
//bool shortSequence = false;
//bool matchSequence = false;
bool orangeFlag = false;
bool lastOrangeFlag = false;
char flag[] = {'o','k','p','p','k'};
char startFlag[] = {'o','k','p','p','k'};
bool flagUp[] = {true, true, true, false, false};
rtc_time_t timing[5];
int timingAmount[] = {0, 5, 6, 9, 10};
int lastTiming = 10;
int lastSequence = 3;
bool updateScreenContent = false;
int switchStartSetting[] = {0, 0, 0, 0};

void createBox(int xPos, int yPos, int width, int height, bool fill) {
  canvas.createCanvas(width, height);
  if(fill) canvas.fillRect(0, 0, width, height, 15);
  else canvas.drawRect(0, 0, width, height, 15);
  canvas.pushCanvas(xPos, yPos, GLOBAL_UPDATE_MODE);
  canvas.deleteCanvas();
  updateScreenContent = true;
}

/**
 * @brief Write text on screen with a border.
 */
void createTextBox(int xPos, int yPos, int width, int height, String text, int fontSize, bool center, int borderThickness) {
  canvas.createCanvas(width, height);
  canvas.drawLine(borderThickness / 2, borderThickness / 2, width - borderThickness / 2, borderThickness / 2, borderThickness, 15);
  canvas.drawLine(width - borderThickness / 2, borderThickness / 2, width - borderThickness / 2, height - borderThickness / 2, borderThickness, 15);
  canvas.drawLine(width - borderThickness / 2, height - borderThickness / 2, borderThickness / 2, height - borderThickness / 2, borderThickness, 15);
  canvas.drawLine(borderThickness / 2, height - borderThickness / 2, borderThickness / 2, borderThickness / 2, borderThickness, 15);
  canvas.setTextSize(fontSize);
  if(center) {
    int textSizeX = 6 * fontSize * text.length();
    int textSizeY = 7 * fontSize;
    canvas.drawString(text, width / 2 - textSizeX / 2, height / 2 - textSizeY / 2);
  } else {
    canvas.drawString(text, 2 * borderThickness, 2 * borderThickness);
  }
  canvas.pushCanvas(xPos, yPos, GLOBAL_UPDATE_MODE);
  canvas.deleteCanvas();
  updateScreenContent = true;
}

/**
 * @brief Write text on screen.
 */
void createText(String text, int xPos, int yPos, int fontSize) {
  int lineAmount = 1;
  int startChar = 0;
  while(text.indexOf("\n", startChar + 2) != -1) {
    startChar = text.indexOf("\n", startChar) + 1;
    lineAmount++;
  }
  String lines[lineAmount];
  int longestLine = 0;
  for(int i = 0; i < lineAmount; i++) {
    lines[i] = text.substring(0, text.indexOf("\n"));
    text = text.substring(text.indexOf("\n") + 1);
    if(lines[i].length() > longestLine) longestLine = lines[i].length();
  }
  lines[lineAmount - 1] = text;
  canvas.createCanvas(6 * fontSize * longestLine, 8 * fontSize * lineAmount);
  canvas.setTextSize(fontSize);
  for(int i = 0; i < lineAmount; i++) {
    canvas.drawString(lines[i], 0, 8 * fontSize * i);
  }
  canvas.pushCanvas(xPos, yPos, GLOBAL_UPDATE_MODE);
  canvas.deleteCanvas();
  updateScreenContent = true;
}

// | 0-1 : buttons + switches | 2 : encoder | 3 : SD-card | 4-10 : GPS |

/**
 * @brief Request hardware state data over I2C.
 * 
 * @details Receives an 11 byte message:
 * | 0 : buttons | 1 : switches | 2 : encoder | 3 : SD-card | 4-10 : GPS |
 */
void getInput() {
  for(int i = 0; i < sizeof(inputData) / sizeof(inputData[0]); i++) {
    lastInputData[i] = inputData[i];
  }
  int input[11];
  int inputLength = sizeof(input) / sizeof(input[0]);
  int n = 0;
  Wire1.requestFrom(8, inputLength);
  delay(50);
  while(Wire1.available() && n < inputLength) {
    input[n] = Wire1.read();
    n++;
  }
//  while(Wire1.available()) {
//    Wire1.read(); // overflow
//  }
  for(int i = 0; i < sizeof(inputData) / sizeof(inputData[0]); i++) {
    inputData[i] = 0;
  }
  inputData[0] += input[0] >= 32;   //Switch 1 (herhaalde start uit/aan)
  input[0] = input[0] % 32;
  inputData[1] += input[0] >= 16;   //Switch 2 (normaal, kort, match sequence)
  input[0] = input[0] % 16;
  inputData[1] += input[0] >= 8;    //Switch 2
  input[0] = input[0] % 8;
  inputData[2] += input[0] >= 4;    //Switch 3 (oranje vlag uit/aan)
  input[0] = input[0] % 4;
  inputData[3] += input[0] >= 2;    //Switch 4 (p, z of zwarte flag)
  input[0] = input[0] % 2;
  inputData[3] += input[0] >= 1;    //Switch 4

  inputData[4] += input[1] >= 16;   //Button 1 (afbreken)
  input[1] = input[1] % 16;
  inputData[5] += input[1] >= 8;    //Button 2 (finisher)
  input[1] = input[1] % 8;
  inputData[6] += input[1] >= 4;    //Button 3 (toeter)
  input[1] = input[1] % 4;
  inputData[7] += input[1] >= 2;    //Button 4 (overtreding)
  input[1] = input[1] % 2;
  inputData[8] += input[1] >= 1;    //Button 5 (start)

  repeated = (inputData[0] == 1);

  if(inputData[1] == 0) {
    timingAmount[0] = 0;
    timingAmount[1] = 5;
    timingAmount[2] = 6;
    timingAmount[3] = 9;
    timingAmount[4] = 10;
  } else if(inputData[1] == 1) {
    timingAmount[0] = 0;
    timingAmount[1] = 3;
    timingAmount[2] = 4;
    timingAmount[3] = 5;
    timingAmount[4] = 6;
  } else {
    timingAmount[0] = 0;
    timingAmount[1] = 2;
    timingAmount[2] = 3;
    timingAmount[3] = 6;
    timingAmount[4] = 7;
  }

  orangeFlag = (inputData[2] == 1);

  if(inputData[3] == 0) {
    flag[2] = 'p';
    flag[3] = 'p';
  } else if(inputData[3] == 1) {
    flag[2] = 'u';
    flag[3] = 'u';
  } else {
    flag[2] = 'z';
    flag[3] = 'z';
  }
  
  aborted = (inputData[4] == 1);
  for(int i = 0; i < sizeof(lastAborts) / sizeof(lastAborts[0]) - 1; i++) {
    lastAborts[i] = lastAborts[i + 1];
    if(lastAborts[i] == 0) {
      aborted = false;
    }
  }
  lastAborts[sizeof(lastAborts) / sizeof(lastAborts[0]) - 1] = inputData[4];

  if(started && aborted) {
    Serial.println("afgebroken");
    sendSetting = 2;
    sendFlag = 10;
  } else if(started && (inputData[4] == 0 && lastInputData[4] == 1 && lastAborts[0] == 0)) {
    Serial.println("onderbroken");
    sendSetting = 2;
    sendFlag = 11;
  } else if(inputData[7] == 1 && lastInputData[7] != inputData[7]) {
    Serial.println("finisher");
    sendSetting = 2;
    sendFlag = 12;
  } else if(inputData[5] == 1 && lastInputData[5] != inputData[5]) {
    Serial.println("overtreding");
    sendSetting = 2;
    sendFlag = 13;
  } else if(!started && (inputData[8] == 1 && lastInputData[8] != inputData[8])) {
    Serial.println("start");  
    sendSetting = 2;
    sendFlag = 14;
  } else if(inputData[6] == 1 && lastInputData[6] != inputData[6]) {
    Serial.println("toeter");
    sendSetting = 2;
    sendFlag = 15;
  }

  encoderChange = input[2];   //Encoder change
  if(input[2] > 127) encoderChange = -(256 - input[2]);   //Check if the encoder change is positive or negative
  startTime.min += encoderChange;
  while(startTime.min >= 60) {
    startTime.min -= 60;
    startTime.hour++;
    while(startTime.hour >= 24) {
      startTime.hour -= 24;
    }
  }
  while(startTime.min < 0) {
    startTime.min += 60;
    startTime.hour--;
    while(startTime.hour < 0) {
      startTime.hour += 24;
    }
  }
  SDCard = input[3];

  if(input[10]) {   //GPS fix
    RTC.setDate(input[4], input[5], input[6]);
    RTC.setTime(input[7], input[8], input[9]);
    char timeBuffer[32];
    sprintf(timeBuffer,"%02d/%02d/%04d %02d:%02d:%02d", input[4], input[5], input[6], input[7], input[8], input[9]);
    Serial.println(timeBuffer);
  }
}

/**
 * @brief Create List of booleans determening the LED state of buttons & switches.
 */
void makeOutput(bool* output) { 
//  if((inputData[0] == 0) == repeated) output[0] = true;
//  if((inputData[2] == 0) == !orangeFlag) output[2] = true;
//  if(startFlag[3] != flag[3]) output[3] = true;
//  if(!started) output[4] = true;
//  if(sendHorn) output[5] = true;
  if(inputData[0] != switchStartSetting[0]) output[0] = true;
  if(inputData[1] != switchStartSetting[1]) output[1] = true;
  if(inputData[2] != switchStartSetting[2]) output[2] = true;
  if(inputData[3] != switchStartSetting[3]) output[3] = true;
  if(!started) output[4] = true;
  if(sendHorn) output[5] = true;
}

/**
 * @brief Sends hardware state data over I2C.
 * 
 * sends an 12 byte message:
 * | 0-5 : button & switches LED state | 6 : audio frament | 7 : settings state | 8-10 : time | 11 : flag | 
 */
void sendOutput(bool* output) {
  Wire1.beginTransmission(8);
  for(int i = 0; i < 6; i++) {
    Wire1.write(output[i]);
  }
  if(sendSoundFragment) {
    Wire1.write(soundFragment);
    sendSoundFragment = false;
    currentSoundFragment = soundFragment;
  } else {
    Wire1.write(99);
  }
  Wire1.write(sendSetting);
  if(sendSetting == 1) {
    rtc_date_t dateNow = RTC.getDate();
    sendTime.hour = dateNow.day;
    sendTime.min = dateNow.mon;
    sendTime.sec = dateNow.year;
  } else sendTime = RTC.getTime();
  Wire1.write(sendTime.hour);
  Wire1.write(sendTime.min);
  Wire1.write(sendTime.sec);
  Wire1.write(sendFlag);
  sendSetting = 0;
  Wire1.endTransmission();
  delay(25);
}

/**
 * @brief Display current time in the corner of the Epaper screen. 
 */
void showTime() {
  if(RTC.compareTime(lastTime.hour, lastTime.min, lastTime.sec)) { 
    lastTime = RTC.getTime();
    char timeBuffer[32];
    sprintf(timeBuffer,"%02d:%02d:%02d", lastTime.hour, lastTime.min, lastTime.sec);
    createTextBox(0, 0, WIDTH / 8 * 3, HEIGHT / 24 * 7, timeBuffer, 6, true, 10);
  }
}

/**
 * @brief Create sequence instruction string. 
 * 
 * @param index Step index from the sequence.
 * @return String 
 */
String getLine(int index) { 
  char tempFlag[5];
  if(started) {
    for(int i = 0; i < 5; i++) {
      tempFlag[i] = startFlag[i];
    }
  } else {
    for(int i = 0; i < 5; i++) {
      tempFlag[i] = flag[i];
    }
  }
  if(index >= sizeof(tempFlag) / sizeof(tempFlag[0]) || index >= sizeof(flagUp) / sizeof(flagUp[0])) return "No flag";
  String line = "";
  if(tempFlag[index] == 'o') line += "Oranje";
  else if(tempFlag[index] == 'k') line += "Klasse";
  else if(tempFlag[index] == 'p') line += "Papa";
  else if(tempFlag[index] == 'u') line += "Uniform";
  else line += "Zwarte";
  line += " vlag ";
  if(flagUp[index]) line += "omhoog";
  else line += "omlaag";
  return line;
}

/**
 * @brief Display the sequence overview.
 */
void showOverview() {
  bool updateText = false;
  String lines[sizeof(flag) / sizeof(flag[0])];
  for(int i = 0; i < sizeof(flag) / sizeof(flag[0]); i++) {
    lines[i] = getLine(i);
    if(lastLines[i] != lines[i]) {
      lastLines[i] = lines[i];
      updateText = true;
    }
  }
  if( lastRepeated != repeated || 
      lastOrangeFlag != orangeFlag || 
      lastSequence != inputData[1] || 
      updateText || 
      lastFlagTiming.hour != startTime.hour || 
      lastFlagTiming.min != startTime.min ||
      lastSDCard != SDCard) {
    String variablesText = "Herhaald: ";
    if(repeated) variablesText += "aan";
    else variablesText += "uit";
    variablesText += "\nSequence: ";
    if(inputData[1] == 0) variablesText += "normaal (5-4-1-0)  ";
    else if(inputData[1] == 1) variablesText += "kort (3-2-1-0)     ";
    else variablesText += "match (5-4-(2)-1-0)";
    variablesText += "\nOranje vlag: ";
    if(orangeFlag) variablesText += "aan";
    else variablesText += "uit";
    
    for(int i = 0 + (!orangeFlag); i < sizeof(lines) / sizeof(lines[0]); i++) {
      rtc_time_t flagTime = RTC.addTime(startTime, RTC.Time(0, timingAmount[i], 0));
      variablesText += "\n" + RTC.toStringTime(flagTime).substring(0, 5) + "  " + lines[i] + " ";
    }
    if(!orangeFlag) variablesText += "\n";
    
    lastRepeated = repeated;
    lastOrangeFlag = orangeFlag;
    lastSequence = inputData[1];
    lastFlagTiming.hour = startTime.hour;
    lastFlagTiming.min = startTime.min;
    lastSDCard = SDCard;
    
    createText(variablesText, 40, HEIGHT / 10 * 3, 3);
    if(!SDCard) createText("Geen SD", WIDTH / 10 * 7, HEIGHT / 10 * 3, 5);
    else createText("       ", WIDTH / 10 * 7, HEIGHT / 10 * 3, 5);
  }
}

void showStartTime() {
  if(startTime.hour != lastStartTime.hour || startTime.min != lastStartTime.min || startTime.sec != lastStartTime.sec || timingAmount[4] != lastTiming) {
    lastStartTime.hour = startTime.hour;
    lastStartTime.min = startTime.min;
    lastStartTime.sec = startTime.sec;
    lastTiming = timingAmount[4];
    createTextBox(WIDTH / 8 * 3, 0, WIDTH - WIDTH / 8 * 3, HEIGHT / 24 * 7, "Volgende start: " + RTC.toStringTime(RTC.addTime(startTime, RTC.Time(0, timingAmount[4], 0))), 4, true, 10);
  }
}

/**
 * @brief Get index of the current step of the sequence.
 */
int getStage() {
  int stage = 0;
  for(int i = 0; i < 5; i++) {
    if(RTC.compareTime(RTC.getTime(), timing[i]) >= 0) stage = i;
  }
  return stage;
}

/**
 * @brief Displays the next instruction on the Epaper screen.
 */
void showNext() {
  int stage = getStage();
//  if(stage == 0 && RTC.compareTime(RTC.getTime(), timing[0]) < 0) {
//    stage = -1;
//  }
  if(RTC.compareTime(RTC.getTime(), timing[stage]) < 0) {
    stage = -1 + !orangeFlag;
  }
  String nextFlagString = RTC.toStringTime(timing[(stage + 1) % 5]) + "  " + getLine((stage + 1) % 5);
  if(nextFlagString != lastNextFlagString) {
    createTextBox(WIDTH / 8 * 3, 0, WIDTH - WIDTH / 8 * 3, HEIGHT / 24 * 7, nextFlagString , 3, true, 10);
    lastNextFlagString = nextFlagString;
  }
}

/**
 * @brief Display the current instruction on the Epaper screen.
 */
void showCurrent() {
  int stage = getStage();
  String currentString = getLine(stage);
  if((stage == 0 && !orangeFlag) || RTC.compareTime(RTC.getTime(), timing[stage]) < 0) {
    currentString = "Wacht op volgende vlag";
  }
  if(currentString != lastCurrentString) {
    createTextBox(0, HEIGHT / 24 * 7, WIDTH, HEIGHT - HEIGHT / 24 * 7, currentString, 5, true, 10);
    lastCurrentString = currentString;
  }
  if(lastSDCard != SDCard) {
    if(!SDCard) createText("Geen SD", WIDTH / 10 * 7, HEIGHT / 10 * 3, 5);
    else createText("       ", WIDTH / 10 * 7, HEIGHT / 10 * 3, 5);
  }
}

/**
 * @brief Checks & set the global variable `sendHorn` on true 1 (second?) before instruction is to be executed. 
 */
void honk() {
  int stage = getStage();
  if(RTC.compareTime(RTC.getTime(), timing[getStage()]) < 1 && RTC.compareTime(RTC.getTime(), timing[getStage()]) >= 0) {
    sendHorn = true;
  } else {
    sendHorn = false;
  }
}

/**
 * @brief Get the sound fragment of an instruction.
 * 
 * @param flag Flag of the instruction raised 'k', 'p', 'u' or 'z'.
 * @param up Select sound fragment about rasing or lowering.
 * @param typeOfFragment Time step varient (30 seconds: 0, 10 seconds: 1, countdown + flag: 2).
 * @return fragment index (maybe). 
 */
int getFragment(char flag, bool up, int typeOfFragment) {
  int frag = typeOfFragment;
  if(!up) frag += 3;
  if(flag == 'k') frag += 6;
  else if(flag == 'p') frag += 12;
  else if(flag == 'u') frag += 18;
  else if(flag == 'z') frag += 24;
  return frag;
}

/**
 * @brief Calculate the remaining seconds of an instruction (maybe).
 * 
 * @param flag Flag of the instruction raised 'k', 'p', 'u' or 'z'.
 * @param up Select sound fragment about rasing or lowering.
 * @param typeOfFragment Time step varient (30 seconds: 0, 10 seconds: 1, countdown + flag: 2).
 * @return Remaining time in seconds.
 */
int getLoopTime(char flag, bool up, int typeOfFragment) {
  int frag = getFragment(flag, up, typeOfFragment);
  float sec = ((frag - currentSoundFragment + totalSoundFragments) % totalSoundFragments) * 0.2;
  return int(round(sec));
}

/**
 * @brief Sets the global variables `sendSoundFragment` & `soundFragment` depending on if an audio frament schould be played.
 * 
 * @details This function selects the correct audio frament and saves it in global variables
 * so it can be passed & played from the Arduino. The sending of the data is done via the
 * `SendOutput()` function.
 *  
 * There are 6 audio files for each flag:
 * - 30 seconds before raising 
 * - 10 seconds before raising 
 * - 5 seconds count down & raise command 
 * - 30 seconds before lowering 
 * - 10 seconds before lowering 
 * - 5 seconds count down & lower command
 */
void playSound() {
  int stage = getStage();
  int secondsToNextStage = -RTC.compareTime(RTC.getTime(), timing[stage]);
  if(secondsToNextStage < 0){
    stage = (stage + 1) % (sizeof(flag) / sizeof(flag[0]));
    secondsToNextStage = -RTC.compareTime(RTC.getTime(), timing[stage]);
  }
  if(stage != lastStage) {
    Serial.println("fragmentsplayed reset");
    fragmentPlayed[0] = false;
    fragmentPlayed[1] = false;
    fragmentPlayed[2] = false;
    fragmentsPlayed = 0;
    lastStage = stage;
  }
  if(fragmentPlayed[0] == false && secondsToNextStage == 30 + getLoopTime(startFlag[stage], flagUp[stage], 0) && fragmentsPlayed < 1) {
    sendSoundFragment = true;
    soundFragment = getFragment(startFlag[stage], flagUp[stage], 0);
    fragmentPlayed[0] = true;
    fragmentsPlayed = 1;
    Serial.println("30 seconden");
    Serial.println(fragmentsPlayed);
  } else if(fragmentPlayed[1] == false && secondsToNextStage == 10 + getLoopTime(startFlag[stage], flagUp[stage], 1) && fragmentsPlayed < 2) {
    sendSoundFragment = true;
    soundFragment = getFragment(startFlag[stage], flagUp[stage], 1);
    fragmentPlayed[1] = true;
    fragmentsPlayed = 2;
    Serial.println("10 seconden");
    Serial.println(fragmentsPlayed);
  } else if(fragmentPlayed[2] == false && secondsToNextStage == 5 + getLoopTime(startFlag[stage], flagUp[stage], 2) && fragmentsPlayed < 3) {
    sendSoundFragment = true;
    soundFragment = getFragment(startFlag[stage], flagUp[stage], 2);
    fragmentPlayed[2] = true;
    fragmentsPlayed = 3;
    Serial.println("5 seconden");
    Serial.println(fragmentsPlayed);
  }
//  Serial.println(secondsToNextStage);
}

/**
 * @brief Change the global variables `sendSetting` & `sendTime` to create a new log file on the SD.
 * 
 * @details The sending of the data is done via the `SendOutput()` function.
 */
void makeFile(rtc_date_t date) {
  sendSetting = 1;
  sendTime.hour = date.day;
  sendTime.min = date.mon;
  sendTime.sec = date.year % 100;
}

void makeLog() {
  int stage = getStage();
  if(RTC.compareTime(RTC.getTime(), timing[getStage()]) < 1 && RTC.compareTime(RTC.getTime(), timing[getStage()]) >= 0) {
    if(!logMade && !(stage == 0 && !orangeFlag)) {
      sendSetting = 2;
      sendTime = RTC.getTime();
      sendFlag = 0;
      if(startFlag[stage] == 'k') sendFlag = 2;
      else if(startFlag[stage] == 'p') sendFlag = 4;
      else if(startFlag[stage] == 'u') sendFlag = 6;
      else if(startFlag[stage] == 'z') sendFlag = 8;
      if(!flagUp[stage]) sendFlag++;
    }
    logMade = true;
  } else {
    logMade = false;
  }
}

/**
 * @brief Resets all the global vars to their starting value.
 */
void resetVars() {
  lastLines[0] = "";
  for(int i = 0; i < 5; i++) {
    startFlag[i] = flag[i];
  }
  for(int i = 0; i < 5; i++) {
    timing[i] = RTC.addTime(startTime, RTC.Time(0, timingAmount[i], 0));
  }
  lastNextFlagString = "";
  lastCurrentString = "";
  lastStartTime = RTC.Time(0,0,0);
  lastLines[0] = "";
  fragmentPlayed[0] = false;
  fragmentPlayed[1] = false;
  fragmentPlayed[2] = false;
  fragmentsPlayed = 0;
  lastStage = -1;
  M5.EPD.Clear(true);
}

void setup() {
  M5.begin(M5Touch, M5SD, M5Serial, M5Battery, M5I2C);
  Wire1.begin(33, 26);
  M5.EPD.Clear(true);
  Serial.begin(115200);
  while(!Serial);
  lastTime = RTC.getTime();
  startTime = RTC.getTime();
  startTime.sec = 0;
  int timingAmount[] = {0, 5, 6, 9, 10};
  for(int i = 0; i < 5; i++) {
    timing[i] = RTC.addTime(startTime, RTC.Time(0, timingAmount[i], 0));
  }
  M5.disableMainPower();
  delay(1500);
  Serial.println("sending file data");
  makeFile(RTC.getDate());
  Serial.println("setup complete");
}

/**
 * @brief Main state logic loop.
 * 
 * @details This function first checks if a date/time is send to 
 * synchronize the Epaper RTC with the GPS time & date.
 * 
 * Second it requests the hardware state from the Arduino.
 * 
 * lastly it handles the states of the program. 
 */
void loop() {
  if(Serial.available()) {
    String dataString = "";
    while(Serial.available()) {
      dataString += (char)Serial.read();
    }
    if(dataString.indexOf("time: ") != -1) {
      RTC.setTime(dataString.substring(6, 9).toInt(), dataString.substring(9, 12).toInt(), dataString.substring(12, 15).toInt());
      Serial.println("time set to: " + RTC.toStringTime(RTC.getTime()));
    }
    if(dataString.indexOf("date: ") != -1) {
      RTC.setDate(dataString.substring(6, 9).toInt(), dataString.substring(9, 12).toInt(), dataString.substring(12, 17).toInt());
      Serial.println("date set to: " + RTC.toStringTime(RTC.getTime()));
    }
  }
  getInput();

  if(!started) {
    showOverview();
    showStartTime();
    if(inputData[8]) {
      for(int i = 0; i < 4; i++) {
        switchStartSetting[i] = inputData[i];
      }
      resetVars();
      started = true;
    }
  } else {
    showNext();
    showCurrent();
    honk();
    makeLog();
    playSound();
    if(repeated && getStage() == 4) {
      for(int i = 0; i < 5; i++) {
        timing[i] = RTC.addTime(RTC.Time(RTC.getTime().hour, RTC.getTime().min, 0), RTC.Time(0, timingAmount[i] - timingAmount[1], 0));
      }
      for(int i = 0; i < 4; i++) {
        switchStartSetting[i] = inputData[i];
      }
    }
    if(lastAborts[sizeof(lastAborts) / sizeof(lastAborts[0]) - 1] == 0 && lastAborts[sizeof(lastAborts) / sizeof(lastAborts[0]) - 2] == 1) {
      resetVars();
      started = false;
    } else if(aborted) {
      createTextBox(0, 0, WIDTH, HEIGHT, "Startprocedure afgebroken", 5, true, 10);
      M5.EPD.UpdateFull(UPDATE_MODE_GL16);
      delay(1500);
      }
  }

  bool output[] = {false, false, false, false, false, false};
  makeOutput(output);
  sendOutput(output); 
  showTime();

  if(updateScreenContent) {
    updateScreenContent = false;
    M5.EPD.UpdateFull(UPDATE_MODE_DU4);
  }
}

// code om uit te proberen:

//String inputs = "S1: " + String(inputData[0]) +   // single/repeated start
//                "S2: " + String(inputData[1]) +   // normaal/kort/match start
//                "S3: " + String(inputData[2]) +   // toeter bij vlag omhoog aan/uit
//                "S4: " + String(inputData[3]) +   // papa/uniform/zwarte vlag
//                "B1: " + String(inputData[4]) +   // afbreken
//                "B2: " + String(inputData[5]) +   // ...
//                "B3: " + String(inputData[6]) +   // toeter
//                "B4: " + String(inputData[7]) +   // ...
//                "B5: " + String(inputData[8]);    // start
//Serial.println(inputs);

// https://docs.m5stack.com/en/api/m5paper/system_api
// voor het starten van de paper zonder de batterij:
// M5.begin(M5Touch, M5SD, M5Serial, false, M5I2C);
// of de globale variable op lijn 10 op false zetten

// zelfde link: info over batterij:
// uint32_t getBatteryRaw()       Read the battery voltage native ADC value
// uint32_t getBatteryVoltage()   Read battery voltage

// zelfde link: shutdown:
// int shutdown( const rtc_time_t &RTC_TimeStruct)
// bijvoorbeeld:
// shutdown(RTC.Time(25, 0, 0);
