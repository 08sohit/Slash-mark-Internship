#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <NewPing.h>

// WiFi credentials
const char* ssid = "your_WiFi_SSID";
const char* password = "your_WiFi_password";

// Relay pin for garage door
#define RELAY_PIN 5

// Ultrasonic sensor pins
#define TRIGGER_PIN 18
#define ECHO_PIN 19
#define MAX_DISTANCE 400  // Maximum distance to measure (in cm)

// Laser pin
#define LASER_PIN 21

// Distance threshold for parking
#define PARKING_DISTANCE 50  // Distance in cm for ideal parking

// Create ultrasonic sensor object
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Initialize laser pin
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Web server to control garage door
  server.on("/open", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY_PIN, HIGH);
    delay(1000); // Simulate opening time
    digitalWrite(RELAY_PIN, LOW);
    request->send(200, "text/plain", "Garage door opened");
  });

  server.on("/close", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY_PIN, HIGH);
    delay(1000); // Simulate closing time
    digitalWrite(RELAY_PIN, LOW);
    request->send(200, "text/plain", "Garage door closed");
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    bool doorStatus = digitalRead(RELAY_PIN);
    String message = doorStatus ? "Garage door is open" : "Garage door is closed";
    request->send(200, "text/plain", message);
  });

  // Endpoint to get parking distance
  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request){
    unsigned int distance = sonar.ping_cm();
    String message = "Distance: " + String(distance) + " cm";
    request->send(200, "text/plain", message);
  });

  server.begin();
}

void loop() {
  // Laser guidance logic
  unsigned int distance = sonar.ping_cm();
  if (distance <= PARKING_DISTANCE) {
    digitalWrite(LASER_PIN, HIGH);  // Turn on the laser when the car is within parking distance
  } else {
    digitalWrite(LASER_PIN, LOW);  // Turn off the laser when the car is not within parking distance
  }
}

// Voice command logic
// For voice commands, you can use services like IFTTT or custom Alexa/Google Assistant skills
// This is a simple example using webhooks and IFTTT

void setupVoiceCommands() {
  server.on("/voice/open", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY_PIN, HIGH);
    delay(1000); // Simulate opening time
    digitalWrite(RELAY_PIN, LOW);
    request->send(200, "text/plain", "Garage door opened via voice command");
  });

  server.on("/voice/close", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY_PIN, HIGH);
    delay(1000); // Simulate closing time
    digitalWrite(RELAY_PIN, LOW);
    request->send(200, "text/plain", "Garage door closed via voice command");
  });
}
