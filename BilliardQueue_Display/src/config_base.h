/// CONFIGURATION
/// Alter and rename to config.h
//#define DEBUG
//#define PREFIX_TOPIC ""

#define STATE_TOPIC "STATUS"
#define COMMAND_TOPIC "COMMAND"


#define SEND_KEEPALIVE
#ifdef SEND_KEEPALIVE
#define KEEPALIVE_INTERVALL (2000)
#endif

//#define SIMULATE_HARDWARE
#ifndef SIMULATE_HARDWARE
// Display
#define DISPLAY_PINS
#define PWM_ADDR_1 0x40
#define PWM_ADDR_2 0x41
#endif

const char *DEVICE_ID = "Display01";

// Update these with values suitable for your network.
const char *ssid = ".....";
const char *password = ".....";
const char *mqtt_server = ".....";