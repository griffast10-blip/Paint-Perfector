#include <Arduino.h>
#include <Stepper.h>
#include <Wire.h>

// ─── Syringe calibration ────────────────────
// 5 ml  = MAX_STEPS  (max dispense per job)
// 10 ml = OPEN_STEPS (retract to prime syringe)
const int  STEPS_PER_REV = 2048;
const long MAX_STEPS      = 1024;  // 5 ml
const long OPEN_STEPS     = 2048;  // 10 ml

// ─── I2C ─────────────────────────────────────
const byte thisAddress = 9;

struct I2cRxStruct {
  uint8_t cPercent;
  uint8_t mPercent;
  uint8_t yPercent;
  uint8_t kPercent;
};

volatile bool newRxData = false;
I2cRxStruct   rxData;

long cSteps, mSteps, ySteps, kSteps;

// ─── Pin assignments ─────────────────────────
const int M1_IN1 = 2,  M1_IN2 = 3,  M1_IN3 = 4,  M1_IN4 = 5;
const int M2_IN1 = 6,  M2_IN2 = 7,  M2_IN3 = 8,  M2_IN4 = 9;
const int M3_IN1 = 10, M3_IN2 = 11, M3_IN3 = 12, M3_IN4 = 13;
const int M4_IN1 = A0, M4_IN2 = A1, M4_IN3 = A2, M4_IN4 = A3;

Stepper motorC(STEPS_PER_REV, M1_IN1, M1_IN3, M1_IN2, M1_IN4);
Stepper motorM(STEPS_PER_REV, M2_IN1, M2_IN3, M2_IN2, M2_IN4);
Stepper motorY(STEPS_PER_REV, M3_IN1, M3_IN3, M3_IN2, M3_IN4);
Stepper motorK(STEPS_PER_REV, M4_IN1, M4_IN3, M4_IN2, M4_IN4);

long percentToSteps(uint8_t pct) {
  if (pct > 100) pct = 100;
  return (long)((MAX_STEPS * pct) / 100.0);
}



void receiveEvent(int numBytes) {
  if (numBytes == sizeof(I2cRxStruct)) {
    Wire.readBytes((uint8_t*)&rxData, sizeof(I2cRxStruct));
    newRxData = true;
  } else {
    while (Wire.available()) Wire.read();
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("4-Stepper — I2C Slave mode"));

  Wire.begin(thisAddress);
  Wire.onReceive(receiveEvent);

  motorC.setSpeed(15);
  motorM.setSpeed(15);
  motorY.setSpeed(15);
  motorK.setSpeed(15);

  homeAllSyringes();
}

void loop() {
  if (newRxData) {
    noInterrupts();
    newRxData = false;
    interrupts();

    if (rxData.cPercent == 0 && rxData.mPercent == 0 && rxData.yPercent == 0 && rxData.kPercent == 0) {
      Serial.println(F("Home command received over I2C"));
      homeAllSyringes();

    } 
    else {
      cSteps = percentToSteps(rxData.cPercent);
      mSteps = percentToSteps(rxData.mPercent);
      ySteps = percentToSteps(rxData.yPercent);
      kSteps = percentToSteps(rxData.kPercent);

      Serial.print("C%: "); Serial.print(rxData.cPercent);
      Serial.print(" -> "); Serial.print(cSteps);
      Serial.print(" | M%: "); Serial.print(rxData.mPercent);
      Serial.print(" -> "); Serial.print(mSteps);
      Serial.print(" | Y%: "); Serial.print(rxData.yPercent);
      Serial.print(" -> "); Serial.print(ySteps);
      Serial.print(" | K%: "); Serial.print(rxData.kPercent);
      Serial.print(" -> "); Serial.println(kSteps);

      motorC.step(-cSteps);
      motorM.step(-mSteps);
      motorY.step(-ySteps);
      motorK.step(-kSteps);
    }
  }
  delay(10);
}

void homeAllSyringes() {
  // Step 1: Push all the way down to guarantee a known bottom position
  Serial.println(F("Pushing all syringes to bottom..."));
  motorC.step(-OPEN_STEPS); Serial.println(F("  C down"));
  motorM.step(-OPEN_STEPS); Serial.println(F("  M down"));
  motorY.step(-OPEN_STEPS); Serial.println(F("  Y down"));
  motorK.step(-OPEN_STEPS); Serial.println(F("  K down"));
  Serial.println(F("All at bottom."));

  delay(500);

  // Step 2: Retract 10 ml to open/prime each syringe
  Serial.println(F("Opening all syringes to 10 ml..."));
  motorC.step(OPEN_STEPS); Serial.println(F("  C open"));
  motorM.step(OPEN_STEPS); Serial.println(F("  M open"));
  motorY.step(OPEN_STEPS); Serial.println(F("  Y open"));
  motorK.step(OPEN_STEPS); Serial.println(F("  K open"));
  Serial.println(F("All syringes open (10 ml). Ready."));
}