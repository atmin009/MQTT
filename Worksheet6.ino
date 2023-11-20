#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN D4 
#define DHTTYPE DHT11
#define LEDPIN D6 
#define WIFI_SSID "BI_LAB"
#define WIFI_PASSWORD "abc12345"
#define MQTT_SERVER "172.20.10.6"
#define DHT_TOPIC "workshop6"
#define LED_TOPIC "LED"

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setupWifi();
void handleMqttMessages(char* topic, byte* payload, unsigned int length);
void ensureMqttConnection();
void sendDhtData();

void setup() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(115200);
  setupWifi();
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(handleMqttMessages);
  dht.begin();
}

void loop() {
  ensureMqttConnection();
  mqttClient.loop();
  sendDhtData();
  delay(5000); 
}

void setupWifi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void handleMqttMessages(char* topic, byte* payload, unsigned int length) {
  String command = "";
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  command.trim();
  if (command == "On") {
    digitalWrite(LEDPIN, HIGH);
  } else if (command == "Off") {
    digitalWrite(LEDPIN, LOW);
  }
}

void ensureMqttConnection() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      mqttClient.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void sendDhtData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (!isnan(humidity) && !isnan(temperature)) {
    String dhtData = "Humidity: " + String(humidity) + "%, Temperature: " + String(temperature) + "°C";
    mqttClient.publish(DHT_TOPIC, dhtData.c_str());
  }
}
