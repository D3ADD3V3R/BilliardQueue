# BilliardQueue

## MiniDoc
### Messages
Die Nachrichten sind im JSON Format und werden an einen zentralen MQTT Broker versendet.

#### Prefix
Es ist nicht möglich ein Prefix für die MQTT Nachrichten zu definieren.
Dies geschieht nicht in der `config.h` mit der Option `PREFIX_TOPIC`.

#### KeepAlive Nachricht
Die Keepalive Nachricht sieht auf allen Geräten gleich aus und wird in das sub-Topic `STATUS` gesendet.

```json
{
    "Type": "KeepAlive",
    "DeviceType": "STRING",
    "DeviceId": "STRING",
    "Capabilities":[ "STRING", "STRING"]
}
```

#### Command Nachricht
Die Command-Nachricht wird vom Counter zu den Anzeigen gesendet um diesen Befehle zu geben. Sie werden an das subTopic `COMMAND` gesendet.

```json
{
    "Type": "KeepAlive",
    "DeviceType": "STRING",
    "DeviceId": "STRING",
    "Capabilities":[ "STRING", "STRING"]
}
```
