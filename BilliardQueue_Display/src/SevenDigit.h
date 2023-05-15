#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Bitmatrix.h"
#include <pgmspace.h>

class SevenDigit{
    private:
        Adafruit_PWMServoDriver pwm1;
        Adafruit_PWMServoDriver pwm2;

        Bitmatrix ledMap = {
            {true, true, true, true, true, true, true},
            {false, false, false, false, false, false, false}
        };
        
    // declare private variables and methods
    public:
        SevenDigit(const uint8_t addr1, const uint8_t addr2);
        SevenDigit(const uint8_t addr1, const uint8_t addr2, const uint8_t colorMode);

        void SetDisplay(char* value);
        void SetDisplay(uint16_t value);
        void SetDigit(uint8_t index, uint8_t value);
    // declare public variables and methods
};