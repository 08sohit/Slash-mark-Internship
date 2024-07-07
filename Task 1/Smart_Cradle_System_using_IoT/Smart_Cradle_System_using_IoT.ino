#include <WiFi.h>
#include <DHT.h>
#include <ESP32Servo.h>

// Define pins
#define SOUND_SENSOR_PIN A0
#define DHT_PIN 2
#define DHT_TYPE DHT11
#define SERVO_PIN 13

// Wi-Fi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);

// Initialize Servo motor
Servo cradleServo;

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// Function to send data to server (implement as needed)
void sendDataToServer(float temperature, float humidity, bool isCrying) {
  // Implement your server communication logic here
  // For example, using HTTP POST requests to send data
}

void setup() {
  // Start serial communication
  Serial.begin(9600);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize Servo motor
  cradleServo.attach(SERVO_PIN);
  cradleServo.write(90); // Initial position
  
  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  // Read sound sensor value
  int soundLevel = analogRead(SOUND_SENSOR_PIN);
  bool isCrying = soundLevel > 500; // Adjust threshold as needed
  
  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Print values to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Crying: ");
  Serial.println(isCrying ? "Yes" : "No");
  
  // Send data to server
  sendDataToServer(temperature, humidity, isCrying);
  
  // Control Servo motor based on cry detection
  if (isCrying) {
    cradleServo.write(45); // Move to one side
    delay(1000);
    cradleServo.write(135); // Move to the other side
    delay(1000);
  } else {
    cradleServo.write(90); // Stop rocking
  }
  
  // Delay between readings
  delay(2000);
}
