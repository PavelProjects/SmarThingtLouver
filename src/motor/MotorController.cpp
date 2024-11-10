#include <motor/MotorController.h>
#include <motor/utils.h>
#include <SmartThing.h>

#define MIN_SPEED 100
#define MAX_SPEED 200

#define TIMEOUT 3000

MotorController::MotorController(){
}

MotorController::~MotorController() {
    stop();
}

MotorController::MotorController(uint8_t motorFirstPin, uint8_t motorSecondPin, uint8_t potPin) {
    _motorFirstPin = motorFirstPin;
    _motorSecondPin = motorSecondPin;
    _potPin = potPin;

    pinMode(_motorFirstPin, OUTPUT);
    pinMode(_motorSecondPin, OUTPUT);
}

void MotorController::setAccuracy(uint16_t accuracy) {
    _accuracy = accuracy;
}

bool MotorController::setPosition(int turnToPosition) {
    int currentState = getPosition();
    if ((currentState > POT_MAX && turnToPosition > POT_MAX ) || (currentState < POT_MIN && turnToPosition < POT_MIN)) {
        return true;
    }
    turnToPosition = constrain(turnToPosition, POT_MIN, POT_MAX);
    if (abs(currentState - turnToPosition) <  _accuracy) {
        return true;
    }

    long started = millis();
    int pwm = 0;
    int dt = 0;
    while (abs(currentState - turnToPosition) > _accuracy && millis() - started < TIMEOUT) {
        currentState = getPosition();
        dt = currentState - turnToPosition;
        delay(10);
        if (dt <= 0) {
            analogWrite(_motorFirstPin, 255);
            analogWrite(_motorSecondPin, LOW);
        } else {
            analogWrite(_motorFirstPin, LOW);
            analogWrite(_motorSecondPin, 255);
        }
    }
    stop();
    bool res =  abs(currentState - turnToPosition) <=  _accuracy;
    return res;
}

void MotorController::setAngle(uint8_t angle) {
    setPosition(map(angle, 0, 180, POT_MIN, POT_MAX));
}

int MotorController::getPosition() {
    static int oldState = 0;
    int currentState = analogRead(_potPin);
    currentState = lpFilter(currentState, oldState, ALPHA);
    oldState = currentState;
    return currentState;
}

void MotorController::stop() {
    analogWrite(_motorFirstPin, LOW);
    analogWrite(_motorSecondPin, LOW);
}