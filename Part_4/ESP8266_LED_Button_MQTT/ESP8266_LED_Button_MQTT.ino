// Raspberry Pi Talking To WiFi Things Part 4
// Use a push button connected to an ESP8266 to toggle a LED on a Raspberry Pi
// on and off, and a push button connected to Pi to toggle a LED on the ESP8266
// on and off.  This uses MQTT to send and receive messages so the Pi must be
// setup to run an MQTT server (mosquitto).
// Author: Tony DiCola
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


// Configuration:
#define WLAN_SSID       "tony-2.4"       // Your WiFi AP.
#define WLAN_PASS       "101tdicola"     // Your WiFi AP password.
#define LED_PIN         2                // Pin connected to the LED.
#define BUTTON_PIN      0                // Pin connected to the button.
#define MQTT_SERVER     "192.168.1.42"   // Address of the Raspberry Pi running MQTT server.
#define MQTT_PORT       1883             // Standard MQTT port 1883.
#define MQTT_USERNAME   ""               // Set to any username for the MQTT server (default none/empty).
#define MQTT_PASSWORD   ""               // MQTT user password (default none/empty).


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);

// Create the feeds that will be used for publishing and subscribing to LED changes.
Adafruit_MQTT_Publish pi_led = Adafruit_MQTT_Publish(&mqtt, "/leds/pi");
Adafruit_MQTT_Subscribe esp8266_led = Adafruit_MQTT_Subscribe(&mqtt, "/leds/esp8266");

// Forward declaration of MQTT_connect function at end of sketch, required because of Arduino bug.
void MQTT_connect();


void setup() {
  Serial.begin(115200);
  delay(10);

  // Setup LED and turn it off.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Setup button as an input with internal pull-up.
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&esp8266_led);

}

void loop() {
  // Grab an initial button press state at the top of the loop since the
  // functions after this will take some time to run and we don't want to
  // miss a button press.
  int button_first = digitalRead(BUTTON_PIN);
  
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // Listen for updates on any subscribed MQTT feeds and process them all.
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription())) {
    // Check if the update is for the /leds/esp8266 topic.
    if (subscription == &esp8266_led) {
      // Grab the message data and print it out.
      char* message = (char*)esp8266_led.lastread;
      Serial.print("/leds/esp8266 got: ");
      Serial.println(message);
      // Check if the message was ON, OFF, or TOGGLE.
      if (strncmp(message, "ON", 2) == 0) {
        // Turn the LED on.
        digitalWrite(LED_PIN, HIGH);
      }
      else if (strncmp(message, "OFF", 3) == 0) {
        // Turn the LED off.
        digitalWrite(LED_PIN, LOW);
      }
      else if (strncmp(message, "TOGGLE", 6) == 0) {
        // Toggle the LED.
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
    }
  }

  // Check if the button has been pressed by looking for a change from high to
  // low signal (with a small delay to debounce).
  delay(20);
  int button_second = digitalRead(BUTTON_PIN);
  if ((button_first == HIGH) && (button_second == LOW)) {
    // Button was pressed!
    Serial.println("Button pressed!");
    // Send 'TOGGLE' to the /leds/pi_led topic.
    pi_led.publish("TOGGLE");
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
