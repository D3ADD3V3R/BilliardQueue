#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "SevenDigit.h"


SevenDigit::SevenDigit(const uint8_t addr1, const uint8_t addr2, const uint8_t colorMode){
    this->pwm1 = Adafruit_PWMServoDriver(addr1);
    this->pwm2 = Adafruit_PWMServoDriver(addr2);
}

void SevenDigit::SetDisplay(char* value){}
void SevenDigit::SetDisplay(uint16_t value){}
void SevenDigit::SetDigit(uint8_t index, uint8_t value){}