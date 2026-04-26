// data to be sent, guarantees 32 bytes total
struct DataStruct {
    int C;              //  2
    int M;              //  2
    int Y;              //  2
    int K;              //  2
    byte padding[24];   // 24
                        //------
                        // 32
};

DataStruct StepData = {0};

bool newStepData = false;

#include <Wire.h>
#include "DFRobot_TCS34725.h"
#include "LiquidCrystal.h"
#include <string.h>
#include <math.h>

const byte thisAddress = 8; // these need to be swapped for the other Arduino
const byte otherAddress = 9;

// timing variables
unsigned long prevUpdateTime = 0;
unsigned long updateInterval = 500;

// syringes subsystem
const int syringeMotorCyanPin = 2;
const int syringeMotorMagentaPin = 3;
const int syringeMotorYellowPin = 4;
const int syringeMotorKeyPin = 5;

// mixer subsystem
const int mixerMotorPin = 6;

// ui/display subsystem
const int button0Pin = A0;
const int button1Pin = A1;
const int button2Pin = A2;
const int button3Pin = A3;

int button0 = 0;
int button1 = 0;
int button2 = 0;
int button3 = 0;

int currScreen = 0;

char currHex[8] = {000000};

LiquidCrystal lcd (13, 12, 11, 10, 9, 8);

// color sensor subsystem
const int colorSensorPin0 = A4;
const int colorSensorPin1 = A5;

DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS,TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_1X);

uint16_t colorTemp, lux;
int C, M, Y, K;
uint16_t red, green, blue, clear;

float rFact = 1450;    //1.9
float gFact = 910;  //1.001
float bFact = 650;    //0.7

int luxCount = 0;

char hexAlphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
int hexScrollCount = 0;
int currHexPos = 0;

uint32_t r, g, b;

//_____________________________________________________________________________________________________________________________
void setup() {
  Serial.begin(9600);
  Serial.println("Begin Setup...");

  if (!tcs.begin())
  {
    Serial.println("Sensor not found ... check your connections");
    while(!tcs.begin())
    {
      tcs.begin();
    }
  }

  Serial.println("Sensor Found");

  Serial.println("Starting I2C Master Arduino...");

  // set up I2C
  Wire.begin(thisAddress); // join i2c bus


  pinMode(button0Pin, INPUT_PULLUP);
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  	
  lcd.begin(16,2);
	lcd.clear();
  lcd.setCursor(0,0);
  sprintf(currHex, "%02X%02X%02X", 00, 00, 00);
}




//_____________________________________________________________________________________________________________________________
void loop() {


  /* This is for testing variables
  Serial.print("C");
  Serial.println(C); // Prints the sensor reading and a newline
  delay(1000); // Wait for a second
  C = C + 35;
  */

  // main switch-case function; each case accounts for its respective screen
  switch(currScreen) {
      case 0:
        // HOME SCREEN
      	//Serial.print("Home screen code start ");
      	//Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
        lcd.print("#");
        lcd.setCursor(1,0);
      	lcd.print(currHex);
      	
      	lcd.setCursor(12,0);
      	lcd.print("HOME");
      	
      	lcd.setCursor(0,1);
      	lcd.print("SCN HEX MIX SET");
      	
      	currScreen = getCurrScreen(0);
        break;

      case 1:
        // RGB SCANNING SCREEN
      	//Serial.print("Scanning screen code start ");
      	//Serial.println(currScreen);

		    lcd.setCursor(0,0);
        lcd.print("#");
        lcd.setCursor(1,0);
      	lcd.print(currHex);
      	
      	lcd.setCursor(0,1);
      	lcd.print("SEL BRI DAR BACK");
      
      	currScreen = getCurrScreen(1);
        break;

      case 2:
        // !!!!!!DEPRECATED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!THIS IS NOT USED ANYMORE!!!!!!!!!
        // HEXADECIMAL LOOKUP SCREEN
      	//Serial.print("Hex lookup screen code start ");
      	//Serial.println(currScreen);
      
		    //lcd.setCursor(0,0);
      	//lcd.print(currHex);
      
      	//lcd.setCursor(0,1);
      	//lcd.print("SCR SEL ENT BACK");
      
      	currScreen = getCurrScreen(2);
        break;

      case 3:
        // HEXADECIMAL ENTRY SCREEN
      	//Serial.print("Hex entry screen code start ");
      	//Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print(currHex);

        lcd.setCursor(10,0);
      	lcd.print("POS:");
        lcd.setCursor(15, 0);
        lcd.print(currHexPos + 1);
      	
      	// there will be a enterCurrHex() function that will
      	// handle manual entry of a new hexadecimal code
      	
      	lcd.setCursor(0,1);
      	lcd.print("SCR NEX     BACK");
      
      	currScreen = getCurrScreen(3);
        break;

      case 4:
        // MIXING SETUP SCREEN
      	//Serial.print("Mixing setup screen code start ");
      	//Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print(currHex);
      	
      	// there will be a prepMix() function that will
      	// handle manual entry of mixing time
      	// this will feed into the larger mix() function
      	// taken care of by the mixer subsystem
      	
      	lcd.setCursor(0,1);
      	lcd.print("SEND    MIX BACK");
      
      	currScreen = getCurrScreen(4);
        break;

      case 5:
        // MIXING PROCESS SCREEN
		    //Serial.print("Mixing process screen code start ");
      	//Serial.println(currScreen);
      
      	lcd.setCursor(0,0);
      	lcd.print("TIMER TICKS..");
      	
      	// there will be a timer function that handles the
      	// counting on this screen and stops when done
      	
      	lcd.setCursor(0,1);
      	lcd.print("STOP CON BACK");
      
      	currScreen = getCurrScreen(5);
        break;

      case 6:
        // SETTINGS SCREEN
      	//Serial.print("Settings screen code start ");
      	//Serial.println(currScreen);
      
		    lcd.setCursor(0,0);
      	lcd.print("SETTINGS");
      	
      	// there will be options to change settings on this screen
      	
      	lcd.setCursor(0,1);
      	lcd.print("XXX XXX XXX BACK");
      
      	currScreen = getCurrScreen(6);
        break;

  	}
}
// end of void loop()






// UI/DISPLAY SUBSYSTEM_____________________________________________________________________________________________________________________________
int getCurrScreen(int prevScreen)
{
  // handles button inputs and button debounce
  // returns the appropriate screen number to currScreen

 	if (prevScreen == 0)
    {
      	button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); lcd.clear(); return 1; }
      	button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); lcd.clear(); return 3; }
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 4; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 6; }
    }
  	if (prevScreen == 1)
    {
        button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); luxCount = 0; colorSensor(); return 1; }
      	button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); incrementLux(1); return 1; }
        button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); incrementLux(2); return 1; }
        button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); incrementLux(0); lcd.clear(); return 0; }
    }
  	if (prevScreen == 2)
    {
      	//button2 = digitalRead(button2Pin);
      	//if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 3; }
      	//button3 = digitalRead(button3Pin);
      	//if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
        lcd.clear(); return 0;
    }
  	if (prevScreen == 3)
    {
        button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); scrollHex(); return 3; }
        button1 = digitalRead(button1Pin);
      	if (!button1) { while(!digitalRead(button1Pin)); nextHexPos(); return 3; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 4)
    {
        button0 = digitalRead(button0Pin);
      	if (!button0) { while(!digitalRead(button0Pin)); updateDataToSend(); transmitData(); return 3; }
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 4; }
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 5)
    {
      	button2 = digitalRead(button2Pin);
      	if (!button2) { while(!digitalRead(button2Pin)); lcd.clear(); return 0; }
    }
  	if (prevScreen == 6)
    {
      	button3 = digitalRead(button3Pin);
      	if (!button3) { while(!digitalRead(button3Pin)); lcd.clear(); return 0; }
    }
  	
  	return prevScreen;
}


void scrollHex()
{
  //hexScrollCount = 

  char currLetter = currHex[currHexPos];

  for (int i = 0; i < 16; i++) {
    if (hexAlphabet[i] == currLetter)
    {
      if (i == 15)
      {
        currLetter = hexAlphabet[0];
        currHex[currHexPos] = currLetter;
        return;
      }
      else
      {
        currLetter = hexAlphabet[i + 1];
        currHex[currHexPos] = currLetter;
        return;
      }
    }
  }
}


void nextHexPos()
{
  if (currHexPos == 5)
  {
    currHexPos = 0;
    return;
  }
  else
  {
    currHexPos += 1;
    return;
  }
}

// end ui/display functions




// COLOR SENSOR SUBSYSTEM_____________________________________________________________________________________________________________________________
void colorSensor() {
  tcs.getRGBC(&red, &green, &blue, &clear);
  colorTemp = tcs.calculateColortemperature(red, green, blue);
  lux = tcs.calculateLux(red, green, blue);

  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);
  Serial.print("\tColor Temp:\t"); Serial.print(colorTemp); Serial.print("K");
  Serial.print("\tLux:\t"); Serial.print(lux);
  Serial.println("\t");

  rgbToHEX(red, green, blue, clear, lux, currHex);

}


void rgbToHEX(uint16_t Red, uint16_t Green, uint16_t Blue, uint16_t Clear, uint16_t Lux, char currHex[8]) {
  float brightness;
  if ((Lux == 65535) || (Lux < 20)) {
    brightness = 0;
  }
  else if (Lux > 625) {
    brightness = 5.2;
    rFact = 1800;
  }
  else if (Lux > 300) {
    brightness = log((float)(Lux - 20)) / 1.75;
  }
  else {
    brightness = log((float)(Lux - 25)) / 2.5;
    bFact = 700;
  }
  
  

  // Convert raw TCS34725 readings into 0–255 RGB
  if (Clear == 0)  return; 
  
  // Normalize each channel by the clear value and lux
  r = ((uint32_t)Red * 255) / ((uint32_t)Clear * 1.0);
  g = ((uint32_t)Green * 255) / ((uint32_t)Clear * 1.0);
  b = ((uint32_t)Blue * 255) / ((uint32_t)Clear * 1.0);
  

  // map to proper 8-bit RGB range + adjust values to compensate for sensor error
  r = (uint8_t)constrain((r*rFact*brightness) / 1700, 0, 255);
  g = (uint8_t)constrain((g*gFact*brightness) / 1700, 0, 255);
  b = (uint8_t)constrain((b*bFact*brightness) / 1700, 0, 255);


  Serial.println();
  Serial.println("Adjusted + Mapped RGB Normalized");
  Serial.print("R:\t"); Serial.print(r, 4);
  Serial.print("\tG:\t"); Serial.print(g, 4);
  Serial.print("\tB:\t"); Serial.print(b, 4);
  Serial.println("\t");


  // save 8 bit values to a singular hexadecimal variable
  sprintf(currHex, "%02X%02X%02X", (uint8_t)r, (uint8_t)g, (uint8_t)b);

  Serial.println();
  Serial.print("currHex: "); Serial.println(currHex);
  rgbToCMYK();
  return;

}


void incrementLux(int incType) {
  if (incType == 0) { luxCount = 0; return; }

  if (incType == 1) {
    luxCount += 1;
    Serial.print("Previous Lux: "); Serial.print(lux); Serial.print("\tLux Counter: "); Serial.println(luxCount);
    rgbToHEX(red, green, blue, clear, lux + luxCount, currHex);
    return;
  }
  else {
    luxCount -= 1;
    Serial.print("Current Lux: "); Serial.print(lux); Serial.print("\tLux Counter: "); Serial.println(luxCount);
    rgbToHEX(red, green, blue, clear, lux + luxCount, currHex);
    return;
  }

}
// end color sensor functions




// SYRINGES SUBSYSTEM_____________________________________________________________________________________________________________________________
void rgbToCMYK() {
  K = 1 - max(max((uint8_t)r, (uint8_t)g), max((uint8_t)g, (uint8_t)b));
  C = (1 - (uint8_t)r - K)/(1 - K);
  M = (1 - (uint8_t)g - K)/(1 - K);
  Y = (1 - (uint8_t)b - K)/(1 - K);
  Serial.print("K: "); Serial.println(K);
  Serial.print("C: "); Serial.println(C);
  Serial.print("M: "); Serial.println(M);
  Serial.print("Y: "); Serial.println(Y);
}

void transmitData() {

    if (newStepData == true) {
        Wire.beginTransmission(otherAddress);
        Wire.write((byte*) &StepData, sizeof(StepData));
        Wire.endTransmission();    // this is what actually sends the data

            // for demo show the data that as been sent
        Serial.println("Sent:");
        Serial.println(StepData.C);
        Serial.println(StepData.M);
        Serial.println(StepData.Y);
        Serial.println(StepData.K);

        newTxData = false;
    }
}

void updateDataToSend() {

    if (millis() - prevUpdateTime >= updateInterval) {
        prevUpdateTime = millis();
        if (newStepData == false) { // ensure previous message has been sent
            // new data to send goes here


            // ________________________________________________________________________________________________________________________________________!!!!!!
            // CMYK data values go HERE to send to slave Arduino
            // rgbToCMYK();  type shit
            StepData.C = C;
            StepData.M = M;
            StepData.Y = Y;
            StepData.K = K;



            newStepData = true;
        }
    }
}

// MIXER SUBSYSTEM_____________________________________________________________________________________________________________________________
void prepMix()
{
  // handles manual entry of mixing time

}

void mix()
{
  // handles physical mixing process, and stopping and starting the mixer
  // works with the timer() function

}

void timer()
{
  // encapsulates the timer

}
// end mixer functions



//_____________________________________________________________________________________________________________________________________________
// for testing: how much free ram do we have?
int freeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
