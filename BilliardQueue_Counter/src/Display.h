#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "config.h"

#ifdef DRAW_DIGITS
  #include "Font/NotoSans_Bold.h"
  #include <OpenFontRender.h>
  #define TTF_FONT NotoSans_Bold
#endif

#define FRAMERATE 50
#define DARKER_GREY 0x18E3



class CounterDisplay{
    private:
        TFT_eSPI disp = TFT_eSPI();
        TFT_eSprite spr;
#ifdef DRAW_DIGITS
        OpenFontRender ofr;
#endif
        bool dislayUnitStatus[DISPLAY_COUNT];
        uint16_t millisPerFrame;
        uint32_t lastFrameTimeStamp;
        void ringMeter(int x, int y, int r, int val, const char *units, uint32_t backgroundColor, uint32_t frontColor);

    public:
        CounterDisplay();
        void Loop();
        void Init();
        void SetCurrentQueuePosition(uint16_t pos);
        void SetOnlineStatus(uint8_t index, bool online);
};