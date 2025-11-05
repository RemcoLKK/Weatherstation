//remove servo code from main.cpp to servo.cpp

#include <servo.hpp>
Servo servoLow;
Servo servoHigh;
int pos = 0; 
int servoPinLow = 2;
int servoPinHigh = 1;

void servoSetup() {
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
    servoLow.setPeriodHertz(50);    
    servoHigh.setPeriodHertz(50);
	servoLow.attach(servoPinLow, 600, 2700);
	servoHigh.attach(servoPinHigh, 600, 2700);
}

void rotateServo(){
    servoLow.write(180);
    servoHigh.write(180);
    delay(2000);
    servoLow.write(0);
    servoHigh.write(0);
    delay(2000);
}
