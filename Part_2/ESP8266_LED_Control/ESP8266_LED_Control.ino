// Raspberry Pi Talking To WiFi Things Part 2
// Control a LED connected to an ESP8266 by sending it commands over a
// simple socket connection.  Also uses a multicast-DNS listener to expose
// the server under the mDNS name 'esp8266-thing.local'.
// Based on the WiFiWebServer example in the ESP8266 Arduino repository.
// Author: Tony DiCola
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>


// Configuration: 
#define WIFI_NAME       "tony-2.4"       // Your WiFi AP.
#define WIFI_PASSWORD   "101tdicola"     // Your WiFi AP password.
#define LED_PIN         2                // Pin connected to the LED.
#define SERVER_PORT     5000             // Port the server will listen for connections.
#define SERVER_NAME     "esp8266-thing"  // mDNS name to use for the server.


// Create an instance of the server listening on the server port.
WiFiServer server(SERVER_PORT);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Setup LED and turn it off.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_NAME);
  
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Setup mDNS responder.
  if (!MDNS.begin(SERVER_NAME)) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
}

void loop() {
  // Check if a client has connected.
  WiFiClient client = server.available();
  if (!client) {
    // No client connected, start the loop over again.
    return;
  }
  
  // Read a line of input.
  // Use a simple character buffer instead of Arduino's String class
  // because String uses dynamic memory which can be problematic with low
  // memory chips.
  char received[5] = {0};
  if (client.readBytesUntil('\n', received, 4) == 0) {
    // Exceeded 1 second timeout waiting for data.
    // Send the client an error and then disconnect them by starting the
    // loop over again (which will clean up and close the client connection).
    client.println("ERROR: Timeout!");
    client.flush();
    return;
  }
  
  // Match the command.
  if (strncmp(received, "ON", 2) == 0) {
    // Turn the LED on!
    digitalWrite(LED_PIN, HIGH);
    client.println("LED ON!");
  }
  else if (strncmp(received, "OFF", 3) == 0) {
    // Turn the LED off!
    digitalWrite(LED_PIN, LOW);
    client.println("LED OFF!");
  }
  else {
    // Unknown command, send an error and exit early to close the connection.
    client.println("ERROR: Unknown command!");
    client.flush();
    return;
  }

  // Client will automatically be disconnected at the end of the loop!
  // Call flush to make sure any queued up data finishes sending to the client.
  client.flush();
}

