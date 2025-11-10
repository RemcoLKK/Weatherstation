// remove servo code from main.cpp to servo.cpp
#include <ldr.hpp>
#include <servo.hpp>
#include <Arduino.h>

Servo servoLow;
Servo servoHigh;
int pos1 = 0; 
int pos2 = 0;
int servoPinLow  = 2;
int servoPinHigh = 1;

// track if the top servo is currently attached
static bool servoHighAttached = false;

void servoSetup() {
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    servoLow.setPeriodHertz(50);    
    servoHigh.setPeriodHertz(50);

    servoLow.attach(servoPinLow,  600, 2700);
    servoHigh.attach(servoPinHigh, 600, 2700);
    servoHighAttached = true;

    servoLow.write(90);
    servoHigh.write(90);
}

void rotateServo() {
    // timing constants
    static const uint32_t RECHECK_INTERVAL_MS = 20000; // 20 s between full checks
    static const uint32_t SERVO_SETTLE_MS     = 250;   // time to reach 90°
    static const uint32_t SHADOW_WAIT_MS      = 3000;  // wait after lifting panel

    // internal state tracking
    // 0 = idle, waiting for next check
    // 1 = moved to 90°, waiting a bit before detach
    // 2 = servo detached, waiting for light to settle, then read LDRs & move
    static uint8_t  state      = 0;
    static uint32_t lastAction = 0;

    uint32_t now = millis();

    switch (state) {

      case 0: // --- idle / waiting for next recheck ---
        if (now - lastAction >= RECHECK_INTERVAL_MS) {
            // time to start a new cycle: go to 90° shadow position

            if (!servoHighAttached) {
                servoHigh.attach(servoPinHigh, 600, 2700);
                servoHighAttached = true;
            }

            servoHigh.write(90);    // lift panel to avoid shadow
            lastAction = now;       // start short "settle" timer
            state = 1;
        }
        break;

      case 1: // --- give servo some time to reach 90°, then detach ---
        if (now - lastAction >= SERVO_SETTLE_MS) {
            // detach at 90° to stop overcorrection / buzzing
            if (servoHighAttached) {
                servoHigh.detach();
                servoHighAttached = false;
            }

            lastAction = now;   // now start the "shadow settle" wait
            state = 2;
        }
        break;

      case 2: // --- waiting for shadow to settle, then read LDR & move ---
        if (now - lastAction >= SHADOW_WAIT_MS) {
            // light should be stable now, read LDRs
            int whereBright = getMostSun(); // 0=N,1=E,2=S,3=W

            if (whereBright == 0) {        // North
                pos1 = 90;
                pos2 = 45;
            } else if (whereBright == 1) { // East
                pos1 = 0;
                pos2 = 45;
            } else if (whereBright == 2) { // South
                pos1 = 90;
                pos2 = 135;
            } else if (whereBright == 3) { // West
                pos1 = 0;
                pos2 = 135;
            }

            // reattach before actually moving to sun position
            if (!servoHighAttached) {
                servoHigh.attach(servoPinHigh, 600, 2700);
                servoHighAttached = true;
            }

            servoLow.write(pos1);
            servoHigh.write(pos2);

            lastAction = now;  // mark time of completed move
            state = 0;         // back to idle until next cycle
        }
        break;
    }
}
