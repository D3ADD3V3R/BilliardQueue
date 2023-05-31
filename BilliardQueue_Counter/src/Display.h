#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <string.h>
#include "config.h"

#define FRAMERATE 50


class CounterDisplay{
    private:
        TFT_eSPI disp = TFT_eSPI();
        TFT_eSprite spr;

        bool displayUnitStatus[DISPLAY_COUNT];
        uint16_t millisPerFrame;
        uint32_t lastFrameTimeStamp;
        bool hasChange;
        void drawRing(uint16_t val);
        void draw_Julia(float c_r, float c_i, float zoom);

    public:
        CounterDisplay();
        void Loop();
        void Init();
        void SetCurrentQueuePosition(uint16_t pos);
        void SetOnlineStatus(uint8_t index, bool online);
        void drawUpdateMessage(uint8_t perc);
};