#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TOKEN "SEMARCOLD_DEMO_TOKEN"

const char* ssid = "Advanced IoT Labs";
const char* password = "Heisenberg1932";
const char mqtt_server[] = "thingsboard.cloud"; 
const char publishTopic[] = "v1/devices/me/telemetry"; 

WiFiClient client;
PubSubClient mqttClient(client);
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
    if (mqttClient.connect("pulseox001", "pulseox", "pul$3ox")) { 
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
  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.loop();
  
  String payload = "{\"bpm\":";
  payload += String(20);
  payload += ",\"spo2\":";
  payload += String(400);
  payload += "}";

  char attributes[1000];
  long now = millis();

  if(now - lastData > 30000){
    lastData = now;
    payload.toCharArray(attributes, 1000);
    mqttClient.publish(publishTopic, attributes);
    Serial.println(attributes);
  }
}