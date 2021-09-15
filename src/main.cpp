#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <MAX30100_PulseOximeter.h>

// #define DUMMY_DATA

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
  pinMode(19, OUTPUT);
  delay(100);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);

#ifndef DUMMY_DATA
  if (!pox.begin()) {
    Serial.println("ERROR: Failed to initialize pulse oximeter");
    for(;;);
  } else {
    Serial.println("MAX30100 initialization is success");
  }
#endif
}

void loop() {
#ifndef DUMMY_DATA
  pox.update();
#endif

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.loop();
  
  long now = millis();

  if(now - lastData > 5000){
    lastData = now;
  
  #ifndef DUMMY_DATA
    float heartRate = pox.getHeartRate();
    uint8_t spo2 = pox.getSpO2();
  #else
    float heartRate = (float)(millis() % 143);
    uint8_t spo2 = (uint8_t)(millis() % 99);
  #endif

    String payload = "{\"bpm\":";
    payload += String(heartRate);
    payload += ",\"spo2\":";
    payload += String(spo2);
    payload += "}";

    char attributes[1000];
    payload.toCharArray(attributes, 1000);
    Serial.println(attributes);

    mqttClient.publish(publishTopic, attributes);
  }
}