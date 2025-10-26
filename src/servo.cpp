//remove servo code from main.cpp to servo.cpp

#include <servo.hpp>
Servo myservo;
int pos = 0; 
int servoPin = 9;

void servoSetup() {
    ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);    
	myservo.attach(servoPin, 600, 2700);
}

void rotateServo(){
    myservo.write(180);
    delay(2000);
    myservo.write(0);
    delay(2000);
}
