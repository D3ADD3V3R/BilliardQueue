/// CONFIGURATION
/// Alter and rename to config.h
#ifndef CONFIG_H
#define CONFIG_H

//#define PREFIX_TOPIC ""
#define DEBUG
//#define DISPLAY_ONLY
#define DRAW_DIGITS

#define STATE_TOPIC "STATUS"
#define COMMAND_TOPIC "COMMAND"


//#define SEND_KEEPALIVE
#ifdef SEND_KEEPALIVE
#define KEEPALIVE_INTERVALL (2000)
#else
#define IGNORE_KEEPALIVE
#endif

//#define SIMULATE_HARDWARE
#ifndef SIMULATE_HARDWARE
// Buttons
#define FORWARD_BUTTON D1
#define MENU_BUTTON D2

// Display
#define TFT_CS        D8
#define TFT_RST       D6 
#define TFT_DC        D2
#endif

#define DISPLAY_COUNT (2)
#define QUEUE_SIZE (10)

#define DEVICE_ID "Counter01"

// Update these with values suitable for your network.
#define USEWIFICONFIG

#define ssid "......"
#define password "......"
#define mqtt_server "......"

#endif