// Raspberry Pi Talking To WiFi Things Part 3
// Use a push button connected to an ESP8266 to toggle a LED on a Raspberry Pi
// on and off, and a push button connected to Pi to toggle a LED on the ESP8266
// on and off.  A special Python server must be running on the Pi and it will listen
// for LED toggle commands from the ESP8266 and send LED toggle commands when its
// button is pressed.
// Author: Tony DiCola
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>


// Configuration:
#define WIFI_NAME       "...WiFi AP..."  // Your WiFi AP.
#define WIFI_PASSWORD   "...password..." // Your WiFi AP password.
#define LED_PIN         2                // Pin connected to the LED.
#define BUTTON_PIN      0                // Pin connected to the button.
#define SERVER_PORT     5000             // Port the server will listen for connections.
#define SERVER_NAME     "esp8266-thing"  // mDNS name to use for the server.
#define PI_ADDRESS      "192.168.1.42"   // IP address (or hostname) of the Raspberry Pi
                                         // to connect to and toggle its LED on button press.

// Create an instance of the server listening on the server port.
WiFiServer server(SERVER_PORT);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Setup LED and turn it off.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Setup button as an input with internal pull-up.
  pinMode(BUTTON_PIN, INPUT_PULLUP);

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
  // Check if the button has been pressed by looking for a change from high to
  // low signal (with a small delay to debounce).
  int button_first = digitalRead(BUTTON_PIN);
  delay(20);
  int button_second = digitalRead(BUTTON_PIN);
  if ((button_first == HIGH) && (button_second == LOW)) {
    // Button was pressed!
    Serial.println("Button pressed!");
    // Send 'toggle_led' command to the Raspberry Pi server.
    WiFiClient pi;
    if (!pi.connect(PI_ADDRESS, SERVER_PORT)) {
      Serial.println("Failed to connect to Pi!");
      return;
    }
    pi.println("toggle_led");
    pi.flush();
    // Now close the connection and continue processing.
    pi.stop();
  }

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
  #define RECEIVED_SIZE 11
  char received[RECEIVED_SIZE] = {0};
  if (client.readBytesUntil('\n', received, RECEIVED_SIZE-1) == 0) {
    // Exceeded 1 second timeout waiting for data.
    // Send the client an error and then disconnect them by starting the
    // loop over again (which will clean up and close the client connection).
    client.println("ERROR: Timeout!");
    client.flush();
    return;
  }

  // Match the command.
  if (strncmp(received, "toggle_led", 10) == 0) {
    // Toggle the LED.
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Serial.println("Toggle LED!");
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
