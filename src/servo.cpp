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

    // servoLow.write(90);
    // servoHigh.write(90);
}

void rotateServo() {
    // timing constants
    static const uint32_t RECHECK_INTERVAL_MS = 20000; // 20 s between full checks
    static const uint32_t SERVO_SETTLE_MS     = 100;   // time to reach position
    static const uint32_t SHADOW_WAIT_MS      = 2000;  // wait after lifting panel

    // states:
    // 0 = idle, waiting for next check
    // 1 = moved high servo to 90°, waiting to detach
    // 2 = waiting for shadow to settle, then read LDRs & move to sun
    // 3 = after moving to sun, wait a bit then detach again
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
            } else {
            }

            // reattach before actually moving to sun position
            if (!servoHighAttached) {
                servoHigh.attach(servoPinHigh, 600, 2700);
                servoLow.attach(servoPinLow, 600, 2700);
                servoHighAttached = true;
            }

            servoLow.write(pos1);
            servoHigh.write(pos2);

            lastAction = now;  // mark time of move
            state = 3;         // go to "final settle then detach"
        }
        break;

      case 3: // --- wait a bit so move finishes, then detach top servo ---
        if (now - lastAction >= SERVO_SETTLE_MS) {
            if (servoHighAttached) {
                servoHigh.detach();
                servoLow.detach();
                servoHighAttached = false;
            }

            // back to idle, wait for the next full cycle
            lastAction = now;
            state = 0;
        }
        break;
    }
}

