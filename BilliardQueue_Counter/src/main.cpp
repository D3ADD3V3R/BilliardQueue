#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef SIMULATE_HARDWARE
#include "Display.h"
#endif

#include "config.h"

void handleOTAEnd();
void handleOTAProgress(unsigned int, unsigned int);
void handleOTAError(ota_error_t);
void handleOTAStart();

/// PROGRAMM SECIFIC STUFF
#define MSG_BUFFER_SIZE (128)

DynamicJsonDocument DeviceDoc(1024);
WiFiClient espClient;
PubSubClient client(espClient);

#ifndef SIMULATE_HARDWARE
CounterDisplay tft = CounterDisplay();
#endif

const char *DEVICE_TYPE = "Counter";
char DeviceDocMsg[MSG_BUFFER_SIZE];
char CommandMessageBuffer[MSG_BUFFER_SIZE];

int currentCount = 0;
long lastReconnectAttemptTime = 0;
long lastKAMsgTime = 0;

void setupWifi()
{
  Serial.print("Connecting WiFi to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  pinMode(MENU_BUTTON, INPUT_PULLUP);
  pinMode(FORWARD_BUTTON, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("\n");
  Serial.println("BilliardQueue - CounterController - V0.1");
  DeviceDoc["Type"] = "KeepAlive";
  DeviceDoc["DeviceType"] = DEVICE_TYPE;
  DeviceDoc["DeviceID"] = DEVICE_ID;
  DeviceDoc["Capabilities"] = JsonArray();

  Serial.println("Device-Data:");
  serializeJsonPretty(DeviceDoc, Serial);
  serializeJson(DeviceDoc, DeviceDocMsg);
  Serial.println("\n----------------------------------------");
  Serial.println("Setup Start");
  Serial.println();
#ifndef DISPLAY_ONLY
  setupWifi();
  Serial.println("---------------------------------------------");
  Serial.printf("Setting MQTT Server Address: %s:1883\n", mqtt_server);
  client.setServer(mqtt_server, 1883);
  Serial.println("Setting Subscriber callback");
  client.setCallback(callback);
  Serial.println("MQTT Setup finished\n------------------------------------");
  Serial.print("Setup OTA ...");
  /**
   * Enable OTA update
   */

  ArduinoOTA.onStart(handleOTAStart);
  ArduinoOTA.onEnd(handleOTAEnd);
  ArduinoOTA.onProgress(handleOTAProgress);
  ArduinoOTA.onError(handleOTAError);

  ArduinoOTA.setHostname(DEVICE_ID);
  ArduinoOTA.begin();
  Serial.println(" finished");
#else
  Serial.println("Device is in DISPLAY_ONLY MODE");
#endif
#ifdef SIMULATE_HARDWARE
  Serial.printf("Max Queue Size is %i\n", QUEUE_SIZE);
  Serial.println("Control is possible by Pressing (N)ext, (R)eset or (M)enu");
#else
  // Init Display
  tft.Init();
#endif

  Serial.println("Setup done");
}

boolean reconnectMQTTNonBlocking()
{
  if (client.connect(DEVICE_ID))
  {

#ifndef SEND_KEEPALIVE
    client.publish(STATE_TOPIC, DeviceDocMsg, false);
// ... and resubscribe
#endif
    client.subscribe(STATE_TOPIC);
  }
  return client.connected();
}

void sendNumberCommand(uint16_t number)
{
  DynamicJsonDocument comDoc(256);
  comDoc["Type"] = "COMMAND";
  comDoc["CommandType"] = "SET_DISPLAY";
  comDoc["Value"] = number;

  serializeJson(comDoc, CommandMessageBuffer);
  client.publish(COMMAND_TOPIC, CommandMessageBuffer, true);
}

void loop()
{
#ifndef DISPLAY_ONLY
  if (!client.connected())
  {
    long now = millis();
    if (now - lastReconnectAttemptTime > 2000)
    {
      Serial.println("Reconnecting to MQTT Broker");
      lastReconnectAttemptTime = now;
      // Attempt to reconnect
      if (reconnectMQTTNonBlocking())
      {
        lastReconnectAttemptTime = 0;
      }
    }
  }
  else
#endif
  {
    client.loop();

#ifdef SEND_KEEPALIVE
    unsigned long now = millis();
    if (now - lastKAMsgTime > KEEPALIVE_INTERVALL)
    {
      lastKAMsgTime = now;
      Serial.println("Publish KeepAlive-Status");
      client.publish(STATE_TOPIC, DeviceDocMsg);
    }
#endif

#ifdef SIMULATE_HARDWARE
    if (Serial.available() > 0)
    {
      char softButton = Serial.read();
      if (softButton == 'N' || softButton == 'n')
      {
        if (currentCount == QUEUE_SIZE)
        {
          currentCount = 1;
        }
        else
        {
          currentCount += 1;
        }
        sendNumberCommand(currentCount);
        Serial.printf("Current Queue Position: %i\n", currentCount);
      }
      else if (softButton == 'R' || softButton == 'r')
      {
        currentCount = 1;
        sendNumberCommand(currentCount);
        Serial.printf("Current Queue Position: %i\n", currentCount);
      }
      else
      {
        Serial.println("Command not implemented");
      }
      Serial.flush();
    }
#endif
  }
#ifndef SIMULATE_HARDWARE
  tft.Loop();
#endif
  digitalWrite(LED_BUILTIN, client.connected());
#ifndef DISPLAY_ONLY
  ArduinoOTA.handle();
#endif
}

void handleOTAEnd()
{
  Serial.println("\nOTA-End");
}

void handleOTAStart()
{
  String type;
  if (ArduinoOTA.getCommand() == U_FLASH)
  {
    type = "sketch";
  }
  else
  { // U_FS
    type = "filesystem";
  }
  Serial.println("Start updating " + type);
}

void handleOTAProgress(unsigned int progress, unsigned int total)
{
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void handleOTAError(ota_error_t error)
{
  Serial.printf("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR)
    Serial.println("Auth Failed");
  else if (error == OTA_BEGIN_ERROR)
    Serial.println("Begin Failed");
  else if (error == OTA_CONNECT_ERROR)
    Serial.println("Connect Failed");
  else if (error == OTA_RECEIVE_ERROR)
    Serial.println("Receive Failed");
  else if (error == OTA_END_ERROR)
    Serial.println("End Failed");
}