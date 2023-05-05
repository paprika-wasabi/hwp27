// Include WiFi libs
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "website.h"

// Add your wifi credentials here
const char* ssid     = "SecretAP";
const char* password = "MySuperSecretPassword";

// exercise.
unsigned long delayStart;
unsigned long duration;
bool delayRunning = false;

// Webserver on port 80 (standard http port)
WiFiServer server(80);

// Variable to store incoiming http request
String request;

// Name of the device (can be used as DNS query in browser)
#define DEVICE_NAME "HWPRobo"

// Pins of motor
#define MOTOR_A1_PIN D1
#define MOTOR_A2_PIN D2
#define MOTOR_B1_PIN D5
#define MOTOR_B2_PIN D6
const uint8_t motorPins[] = {MOTOR_A1_PIN, MOTOR_A2_PIN, MOTOR_B1_PIN, MOTOR_B2_PIN};

// Us pins
#define US1_PIN D8
#define US2_PIN D7
#define US3_PIN D3
const uint8_t usPins[] = {US1_PIN, US2_PIN, US3_PIN};


void setup() {
  // Init serial

  // exercise part.
  delay(3000);
  Serial.begin(2000000);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // drive(true, 1000, 100);
  // turn(true, 1000, 100);
  // turn(false, 1000, 100);

  // Init motor pins as output
  for (size_t i = 0; i < sizeof(motorPins)/sizeof(motorPins[0]); i++) {
    pinMode(motorPins[i], OUTPUT);
    digitalWrite(motorPins[i], LOW);
  }
  
  // Comment if you want that the ESP creates an AP
  // Connect to wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Die IP vom Webserver auf dem seriellen Monitor ausgeben
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP Adress: ");
  Serial.println(WiFi.localIP());

  // Uncomment if you want that the ESP creates an AP
  /*
  // You can remove the password parameter if you want the AP to be open. 
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  */

  // mDNS name resolving
  if (MDNS.begin(DEVICE_NAME)) {
    Serial.println("MDNS started");
  } else {
    Serial.println("Error starting MDNS");
  }

  // Start webserver
  server.begin();
}

void loop() {
  // Handle clients
  handleClient();
  // Update MDNS
  MDNS.update();
}

void handleClient() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)  {  
    return;  
  }
  // Read the first line of the request (we just need this line)
  request = client.readStringUntil('\r');

  // Print request to serial
  Serial.print("request: ");
  Serial.println(request); 

  // print header message
  client.println(header);
  // Check for corresponding get message  
  if (request.indexOf("GET /pollUS") >= 0) {
    Serial.println("Polling");
    long us1 = measureDistance(D8);
    long us2 = measureDistance(D7);
    long us3 = measureDistance(D3);


    // Send US data to website
    client.printf("{\"US1\":%.2d, \"US2\":%.2d, \"US3\":%.2d}", us1, us2, us3);
    
  // Insert code to make the d-pad control working
  // Start by pressing the buttons of the d pad and watch the serial console to see how the get requests look.

  
  // Serve initial Website
  } else {
    // Finish HTTP-Request with a newline (thats cruical)
    client.flush();
    client.println(page);
    client.println();
  }
}


float measureDistance(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
  delay(0.01);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  duration = pulseIn(pin, HIGH, 30000);
  if (duration == 0) {
    return -1;
  }

  return microsecondsToCentimeters(duration); 
}

void turn(bool left, uint16_t time, uint16_t speed) {
  delayRunning = true;
  delayStart = millis();
  while (delayRunning) {
    if ((millis() - delayStart) > time) {
      delayRunning = false;
      break;
    }
    if (left) {
      setMotor(false, speed, 0);
      setMotor(true, speed, 1);
    } else {
      setMotor(true, speed, 0);
      setMotor(false, speed, 1);
    }
  }
  setMotor(true, 0, 0);
  setMotor(true, 0, 1);
}

void drive(bool forward, uint16_t time, uint16_t speed) {
  delayRunning = true;
  delayStart = millis();
  while (delayRunning) {
    if ((millis() - delayStart) > time) {
      delayRunning = false;
      break;
    }
    setMotor(forward, speed, 0);
    setMotor(forward, speed, 1);
  }
  setMotor(forward, 0, 0);
  setMotor(forward, 0, 1);
}

void setMotor(bool forward, bool motorA, uint16_t speed) {
    switch (motorA) {
    case 0: // motor A left.
      if (forward) {
        analogWrite(D2, speed);
        analogWrite(D1, 0);
      } else {
        analogWrite(D2, 0);
        analogWrite(D1, speed);
      }
      break;
    case 1: // motor B right.
      if (forward) {
        analogWrite(D5, speed);
        analogWrite(D6, 0);
      } else {
        analogWrite(D5, 0);
        analogWrite(D6, speed);
      }
      break;
    default:
      break;
  }
}


long microsecondsToCentimeters(long microseconds) {
// The speed of sound is 340 m/s or 29 microseconds per centimetre.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
return microseconds / 29 / 2;
}
