#include "Display.h"

CounterDisplay::CounterDisplay() : spr(&this->disp){}

void CounterDisplay::Init(){
    #ifdef DEBUG
    Serial.println("Initialise Display");
    #endif
    this->disp.init();
    this->disp.setRotation(1);
#ifdef DRAW_DIGITS
  // Loading a font takes a few milliseconds, so for test purposes it is done outside the test loop
  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
    Serial.println("Render initialize error");
    return;
  }
#endif
}

void CounterDisplay::Loop(){
    if((this->lastFrameTimeStamp + this->millisPerFrame) < millis()){
        this->lastFrameTimeStamp = millis();
        this->disp.fillScreen(DARKER_GREY);

    }
}

void CounterDisplay::ringMeter(int x, int y, int r, int val, const char *units, uint32_t backgroundColor, uint32_t frontColor)
{
  static uint16_t last_angle = 30;

    last_angle = 30;
    disp.fillCircle(x, y, r, DARKER_GREY);
    disp.drawSmoothCircle(x, y, r, TFT_SILVER, DARKER_GREY);
    uint16_t tmp = r - 3;
    disp.drawArc(x, y, tmp, tmp - tmp / 5, last_angle, 330, TFT_BLACK, DARKER_GREY);

  r -= 3;

  // Range here is 0-100 so value is scaled to an angle 30-330
  int val_angle = map(val, 0, 100, 30, 330);


  if (last_angle != val_angle) {
    // Could load the required font here
    //if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
    //  Serial.println("Render initialize error");
    //  return;
    //}
#ifdef DRAW_DIGITS
    ofr.setDrawer(spr); // Link renderer to sprite (font will be rendered in sprite spr)

    // Add value in centre if radius is a reasonable size
    if ( r >= 25 ) {
      // This code gets the font dimensions in pixels to determine the required the sprite size
      ofr.setFontSize((6 * r) / 4);
      ofr.setFontColor(TFT_WHITE, DARKER_GREY);


      // The OpenFontRender library only has simple print functions...
      // Digit jiggle for chaging values often happens with proportional fonts because
      // digit glyph width varies ( 1 narrower that 4 for example). This code prints up to
      // 3 digits with even spacing.
      // A few experiemntal fudge factors are used here to position the
      // digits in the sprite...
      // Create a sprite to draw the digits into
      uint8_t w = ofr.getTextWidth("444");
      uint8_t h = ofr.getTextHeight("4") + 4;
      spr.createSprite(w, h + 2);
      spr.fillSprite(DARKER_GREY); // (TFT_BLUE); // (DARKER_GREY);
      char str_buf[8];         // Buffed for string
      itoa (val, str_buf, 10); // Convert value to string (null terminated)
      uint8_t ptr = 0;         // Pointer to a digit character
      uint8_t dx = 4;          // x offfset for cursor position
      if (val < 100) dx = ofr.getTextWidth("4") / 2; // Adjust cursor x for 2 digits
      if (val < 10) dx = ofr.getTextWidth("4");      // Adjust cursor x for 1 digit
      while ((uint8_t)str_buf[ptr] != 0) ptr++;      // Count the characters
      while (ptr) {
        ofr.setCursor(w - dx - w / 20, -h / 2.5);    // Offset cursor position in sprtie
        ofr.rprintf(str_buf + ptr - 1);              // Draw a character
        str_buf[ptr - 1] = 0;                        // Replace character with a null
        dx += 1 + w / 3;                             // Adjust cursor for next character
        ptr--;                                       // Decrement character pointer
      }
      spr.pushSprite(x - w / 2, y - h / 2); // Push sprite containing the val number
      spr.deleteSprite();                   // Recover used memory

      // Make the TFT the print destination, print the units label direct to the TFT
      ofr.setDrawer(disp);
      ofr.setFontColor(TFT_GOLD, DARKER_GREY);
      ofr.setFontSize(r / 2.0);
      ofr.setCursor(x, y + (r * 0.4));
      ofr.cprintf("Watts");
    }
#endif

    //ofr.unloadFont(); // Recover space used by font metrics etc

    // Allocate a value to the arc thickness dependant of radius
    uint8_t thickness = r / 5;
    if ( r < 25 ) thickness = r / 3;

    // Update the arc, only the zone between last_angle and new val_angle is updated
    if (val_angle > last_angle) {
      disp.drawArc(x, y, r, r - thickness, last_angle, val_angle, TFT_SKYBLUE, TFT_BLACK); // TFT_SKYBLUE random(0x10000)
    }
    else {
      disp.drawArc(x, y, r, r - thickness, val_angle, last_angle, TFT_BLACK, DARKER_GREY);
    }
    last_angle = val_angle; // Store meter arc position for next redraw
  }
}