#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "LittleFS.h" // LittleFS is declared

#include "config.h"

/// PROGRAMM SECIFIC STUFF
#define MSG_BUFFER_SIZE (128)

DynamicJsonDocument DeviceDoc(1024);
DynamicJsonDocument DeviceConfig(1024);
WiFiClient espClient;
PubSubClient client(espClient);

#ifndef SIMULATE_HARDWARE
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(PWM_ADDR_1);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(PWM_ADDR_2);
#endif

const char *DEVICE_TYPE = "Display";
char DeviceDocMsg[MSG_BUFFER_SIZE];
int currentCount = 0;
long lastReconnectAttemptTime = 0;
long lastKAMsgTime = 0;

void setupWifi()
{
  Serial.print("Connecting WiFi to ");
  Serial.printf("%s\n",DeviceConfig["DeviceSSID"]);

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

void setupConfig()
{
  Serial.println("------------------------------");
  Serial.println("Config Assistent");
  Serial.println("Gefundene WLAN");
  WiFi.disconnect();
  bool dataCorrect = false;
  while (!dataCorrect)
  {
    int n = WiFi.scanNetworks();
    int selectedId = 0;
    if (n == 0)
    {
      Serial.println("No Networks found");
      selectedId = 99;
    }
    else
    {
      for (int i = 0; i < n; ++i)
      {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
        delay(10);
      }
      Serial.println("Bitte wählen sie eine ID:");
    }
    if (selectedId == 99)
    {
      Serial.println("Bitte geben sie die Id ein:");
      if (Serial.available() > 1)
      {
        String ssid = Serial.readString();
        DeviceConfig["DeviceSSID"] = ssid;
      }
    }
    else
    {
      DeviceConfig["DeviceSSID"] = WiFi.SSID(selectedId);
    }

    Serial.printf("Bitte geben sie das Passwort ein für [%s]:\n", DeviceConfig["DeviceSSID"]);
    if (Serial.available() > 1)
    {
      String pw = Serial.readString();
      DeviceConfig["DevicePW"] = pw;
    }
    Serial.printf("Daten werden gespeichert: [%s] -> [%s]\n", DeviceConfig["DeviceSSID"], DeviceConfig["DevicePW"]);
    Serial.println("Sind diese Daten korrekt [J/N]?");

    if (Serial.available() > 0)
    {
      char result = Serial.read();
      if (result == 'J' || result == 'j')
      {
        dataCorrect = true;
      }
    }
  }
  Serial.println("Speichere Daten");
  dataCorrect = false;
  while (!dataCorrect)
  {
    Serial.println("Bitte geben sie die Adresse ihres MQTT Brokers ein:");

    if (Serial.available() > 1)
    {
      String mqtt = Serial.readString();
      DeviceConfig["MqttBroker"] = mqtt;
    }

    Serial.printf("Daten werden gespeichert: [%s] \n", DeviceConfig["MqttBroker"]);
    Serial.println("Sind diese Daten korrekt [J/N]?");

    if (Serial.available() > 0)
    {
      char result = Serial.read();
      if (result == 'J' || result == 'j')
      {
        dataCorrect = true;
      }
    }
  }
  Serial.println("Speichere Daten");
  File configFile = LittleFS.open("sys_config.json", "w");
  serializeJson(DeviceConfig, configFile);
  configFile.close();
  Serial.println("Gespeichert");
  Serial.println("------------------------------");
}

void setDisplay(uint16_t number)
{
#ifdef SIMULATE_HARDWARE
  Serial.printf("Setting Display to %i\n", number);
#else
  // Check Numbers

#endif
}

void callback(char *topic, byte *payload, unsigned int length)
{
#ifdef DEBUG
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
#endif

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  if (doc["Type"] == "COMMAND")
  {
    if (doc["CommandType"] == "SET_DISPLAY")
    {
      setDisplay(doc["Value"]);
    }
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial.println("\n");
  Serial.println("BilliardQueue - DisplayController - V0.1");
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
  Serial.println("Init FS");
  LittleFS.begin();

  if (LittleFS.exists("sys_config.json"))
  {
    File configFile = LittleFS.open("sys_config.json", "r");
    deserializeJson(DeviceConfig, configFile);
    configFile.close();
    Serial.println("Config erfolgreich geladen");
  }
  else
  {
    Serial.println("Keine Config gefunden ... starte Einrichtung");
    setupConfig();
  }

  client.setCallback(callback);

#ifndef SIMULATE_HARDWARE
  Serial.println("Initialise Hardware Drivers");
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
    client.subscribe(COMMAND_TOPIC);
  }
  return client.connected();
}

void loop()
{
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
  {
    client.loop();

#ifdef SEND_KEEPALIVE
    unsigned long now = millis();
    if (now - lastKAMsgTime > KEEPALIVE_INTERVALL)
    {
      lastKAMsgTime = now;
#ifdef DEBUG
      Serial.println("Publish KeepAlive-Status");
#endif
      client.publish(STATE_TOPIC, DeviceDocMsg);
    }
#endif
  }
  digitalWrite(LED_BUILTIN, client.connected());
}