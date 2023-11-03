#include <Arduino.h>

#ifndef MotorController_H
#define MotorController_H

#define POT_ACCURACY 100

#define POT_MIN 100
#define POT_MAX 4000

#define SMOOTH_START 0

#define MOTOR_CONTROLLER_TAG "motor_controller"

class MotorController {
private:
    uint8_t _motorFirstPin;
    uint8_t _motorSecondPin;
    uint8_t _potPin;
    uint16_t _accuracy = POT_ACCURACY;

    void setDirection(int dt);
public:
    MotorController(uint8_t motorFirstPin, uint8_t motorSecondPin, uint8_t potPin);
    MotorController();
    ~MotorController();
    void setAccuracy(uint16_t accuracy);

    void setAngle(uint8_t angle);

    // returns true if position was achived
    bool setPosition(int turnToPosition);
    int getPosition();
    void stop();
};

#endif