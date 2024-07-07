#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_BME280.h>
#include <Adafruit_LSM303_U.h>

// Constants
#define GPS_BAUDRATE 9600
#define MQTT_SERVER "your_mqtt_broker_address"
#define MQTT_PORT 1883
#define MQTT_TOPIC "sensor_data"
#define BME_ADDRESS 0x76

// Objects
TinyGPSPlus gps;
SoftwareSerial gpsSerial(16, 17); // RX_PIN, TX_PIN (connect to NEO-6M GPS module)
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_BME280 bme;
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

// Function prototypes
void setupWifi();
void publishSensorData(double lat, double lon);

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPS_BAUDRATE);
  
  setupWifi(); // Connect to WiFi
  
  client.setServer(MQTT_SERVER, MQTT_PORT);

  if (!bme.begin(BME_ADDRESS)) {
    Serial.println("Could not find BME280 sensor!");
    while (1); // Halt the program if the BME280 sensor is not found
  }

  if (!mag.begin()) {
    Serial.println("Could not find LSM303 magnetometer sensor!");
    while (1); // Halt the program if the LSM303 sensor is not found
  }
}

void loop() {
  // Read GPS data
  if (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        double latitude = gps.location.lat();
        double longitude = gps.location.lng();
        publishSensorData(latitude, longitude); // Publish sensor data with GPS coordinates
      }
    }
  }
  client.loop(); // Maintain MQTT connection
}

void publishSensorData(double lat, double lon) {
  // Read sensor data
  float temperature = bme.readTemperature(); // Read temperature in Celsius
  float pressure = bme.readPressure() / 100.0F; // Read pressure in hPa (hectopascals)

  sensors_event_t event; 
  mag.getEvent(&event);
  float magX = event.magnetic.x;
  float magY = event.magnetic.y;
  float magZ = event.magnetic.z;

  // Create JSON payload
  String payload = "{";
  payload += "\"latitude\":" + String(lat, 6) + ",";
  payload += "\"longitude\":" + String(lon, 6) + ",";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"pressure\":" + String(pressure, 2) + ",";
  payload += "\"magX\":" + String(magX, 2) + ",";
  payload += "\"magY\":" + String(magY, 2) + ",";
  payload += "\"magZ\":" + String(magZ, 2);
  payload += "}";

  // Publish to MQTT topic
  if (client.publish(MQTT_TOPIC, payload.c_str())) {
    Serial.println("Data published successfully!");
  } else {
    Serial.println("Publish failed!");
  }
}

void setupWifi() {
  WiFi.begin("your_WiFi_SSID", "your_WiFi_password");
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected!");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}
