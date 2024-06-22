#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// WiFi configuration
const char* ssid = "your_WiFi_SSID";
const char* password = "your_WiFi_password";

// MQTT configuration
const char* mqtt_server = "your_mqtt_broker_address";
const int mqtt_port = 1883;
const char* mqtt_topic = "sensor_data";

// Sensor pins
#define DHTPIN 4  // Pin connected to DHT22
#define RAIN_SENSOR_PIN 34  // Pin connected to rain sensor

// DHT configuration
#define DHTTYPE DHT22   // DHT 22 (AM2302), AM2321
DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Initialize sensors
  dht.begin();

  // Connect to WiFi
  setupWifi();

  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor data
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float temperature = event.temperature;

  dht.humidity().getEvent(&event);
  float humidity = event.relative_humidity;

  int rainValue = analogRead(RAIN_SENSOR_PIN);
  float rainLevel = map(rainValue, 0, 4095, 0, 100); // Adjust mapping as needed

  // Publish sensor data
  publishSensorData(temperature, humidity, rainLevel);

  delay(5000); // Adjust delay as needed
}

void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishSensorData(float temperature, float humidity, float rainLevel) {
  String payload = "{";
  payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity) + ",";
  payload += "\"rainLevel\":" + String(rainLevel);
  payload += "}";

  client.publish(mqtt_topic, payload.c_str());
}
