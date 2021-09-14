#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <MAX30100_PulseOximeter.h>

const char* ssid = "Advanced IoT Labs";
const char* password = "Heisenberg1932";
const char mqtt_server[] = "thingsboard.cloud";
const char mqtt_clientid[] = "pulseox001";
const char mqtt_username[] = "pulseox";
const char mqtt_pass[] = "pul$3ox";
const char publishTopic[] = "v1/devices/me/telemetry"; 

WiFiClient client;
PubSubClient mqttClient(client);
PulseOximeter pox;

long lastData = 0;

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while( WiFi.status() != WL_CONNECTED){

    delay(500);
    Serial.print(".");   
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!pox.begin(PULSEOXIMETER_DEBUGGINGMODE_NONE)) {
    Serial.println("ERROR: Failed to initialize pulse oximeter");
    for(;;);
  }
}

void reconnect(){
  while(!mqttClient.connected()){
    Serial.print("Attempting MQTT connection ....");
    if (mqttClient.connect(mqtt_clientid, mqtt_username, mqtt_pass)) { 
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println("try again in 5 second");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
}

void loop() {
  pox.update();

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.loop();
  
  long now = millis();

  if(now - lastData > 5000){
    lastData = now;

    String payload = "{\"bpm\":";
    payload += String(pox.getHeartRate());
    payload += ",\"spo2\":";
    payload += String(pox.getSpO2());
    payload += "}";

    char attributes[1000];
    payload.toCharArray(attributes, 1000);
    Serial.println(attributes);

    mqttClient.publish(publishTopic, attributes);
  }
}