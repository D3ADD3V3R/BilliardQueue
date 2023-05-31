#include "Display.h"

#define SCREEN_WIDTH this->disp.width()   //
#define SCREEN_HEIGHT this->disp.height() // Taille de l'écran

CounterDisplay::CounterDisplay() : spr(&this->disp) {}

const uint16_t MAX_ITERATION = 300; // Nombre de couleurs
static float zoom = 0.5;

void CounterDisplay::Init()
{
#ifdef DEBUG
  Serial.println("Initialise Display");
#endif
  this->disp.init();
  this->disp.setRotation(1);
  // this->disp.resetViewport();
  this->hasChange = true;
  this->disp.fillScreen(TFT_BLACK);
  this->disp.setFreeFont(&FreeMono9pt7b);
}

void CounterDisplay::Loop()
{
  if (this->hasChange && (this->lastFrameTimeStamp + this->millisPerFrame) < millis())
  {
    this->hasChange = false;
    this->disp.fillScreen(TFT_BLACK);
  }
}

void CounterDisplay::drawRing(uint16_t val)
{
  uint16_t endAngle = map(val, 0, 100, 0, 360);
  this->disp.drawArc(50, 100, 25, 23, 0, endAngle, TFT_RED, TFT_BLACK);
}

void CounterDisplay::drawUpdateMessage(uint8_t perc)
{
  if ((this->lastFrameTimeStamp + this->millisPerFrame) < millis())
  {
    this->lastFrameTimeStamp = millis();

    this->disp.fillScreen(TFT_BLACK);
    this->disp.setTextColor(TFT_WHITE);
    this->disp.setTextSize(1);         // Textgröße festlegen
    this->disp.setTextDatum(MC_DATUM); // Textausrichtung auf die Mitte setzen

    char *updateMessage = "Update in Progress";

    uint8_t yPos = (this->disp.height() - this->disp.fontHeight(2)) / 2;
    uint8_t xPos = (this->disp.width() - this->disp.textWidth(updateMessage)) / 2;

    this->disp.setCursor(xPos, yPos);
    this->disp.print(updateMessage);

    yPos = this->disp.height() - 25;
    sprintf(updateMessage, "Progress: %i\%", perc);
    
    xPos = (this->disp.width()-100) / 2;
    this->disp.drawWideLine(xPos, yPos - 10, xPos + perc, yPos - 10, 5, TFT_CYAN);
    xPos = (this->disp.width() - this->disp.textWidth(updateMessage)) / 2;
    this->disp.setCursor(xPos, yPos);
    this->disp.print(updateMessage);
  }
}

void CounterDisplay::draw_Julia(float c_r, float c_i, float zoom)
{

  this->disp.setCursor(0, 0);
  float new_r = 0.0, new_i = 0.0, old_r = 0.0, old_i = 0.0;

  /* Pour chaque pixel en X */

  for (int16_t x = SCREEN_WIDTH / 2 - 1; x >= 0; x--)
  { // Rely on inverted symettry
    /* Pour chaque pixel en Y */
    for (uint16_t y = 0; y < SCREEN_HEIGHT; y++)
    {
      old_r = 1.5 * (x - SCREEN_WIDTH / 2) / (0.5 * zoom * SCREEN_WIDTH);
      old_i = (y - SCREEN_HEIGHT / 2) / (0.5 * zoom * SCREEN_HEIGHT);
      uint16_t i = 0;

      while ((old_r * old_r + old_i * old_i) < 4.0 && i < MAX_ITERATION)
      {
        new_r = old_r * old_r - old_i * old_i;
        new_i = 2.0 * old_r * old_i;

        old_r = new_r + c_r;
        old_i = new_i + c_i;

        i++;
      }
      /* Affiche le pixel */
      if (i < 100)
      {
        this->disp.drawPixel(x, y, this->disp.color565(255, 255, map(i, 0, 100, 255, 0)));
        this->disp.drawPixel(SCREEN_WIDTH - x - 1, SCREEN_HEIGHT - y - 1, this->disp.color565(255, 255, map(i, 0, 100, 255, 0)));
      }
      if (i < 200)
      {
        this->disp.drawPixel(x, y, this->disp.color565(255, map(i, 100, 200, 255, 0), 0));
        this->disp.drawPixel(SCREEN_WIDTH - x - 1, SCREEN_HEIGHT - y - 1, this->disp.color565(255, map(i, 100, 200, 255, 0), 0));
      }
      else
      {
        this->disp.drawPixel(x, y, this->disp.color565(map(i, 200, 300, 255, 0), 0, 0));
        this->disp.drawPixel(SCREEN_WIDTH - x - 1, SCREEN_HEIGHT - y - 1, this->disp.color565(map(i, 200, 300, 255, 0), 0, 0));
      }
      yield();
    }
  }
}