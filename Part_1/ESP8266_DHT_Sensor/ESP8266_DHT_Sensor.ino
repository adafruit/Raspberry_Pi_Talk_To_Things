// Raspberry Pi Talking To WiFi Things Part 1
// Use an ESP8266 connected to a DHT temperature & humidity sensor
// to log its sensor readings to a Raspberry Pi over WiFi.
// Based on the WiFiClientBasic example in the ESP8266 Arduino repository.
// Author: Tony DiCola
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "DHT.h"


// Configuration:
#define WIFI_NAME       "your AP name"      // Your WiFi AP.
#define WIFI_PASSWORD   "your AP password"  // Your WiFi AP password.
#define PI_IP_ADDRESS   "192.168.1.42"      // IP address of the listening Pi.
#define NAME            "esp8266_dht"       // Name of the device, written as the
                                            // first CSV value.
#define PI_PORT         5000                // Port for the listening Pi.
#define DHTPIN          2                   // DHT sensor signal pin.
#define DHTTYPE         DHT22               // DHT sensor type (DHT11, DHT22,
                                            // or DHT21).  The AM2302 is a DHT22
                                            // and the AM2301 is a DHT21.


DHT dht(DHTPIN, DHTTYPE);
ESP8266WiFiMulti WiFiMulti;

void setup() {
    Serial.begin(115200);
    delay(10);

    dht.begin();

    // We start by connecting to a WiFi network
    WiFiMulti.addAP(WIFI_NAME, WIFI_PASSWORD);

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void loop() {
    const uint16_t port = PI_PORT;
    const char * host = PI_IP_ADDRESS; // ip or dns

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }

    // Take DHT sensor readings.
    float h = dht.readHumidity();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Send the sensor data to the server as a comma separated list.
    client.print(NAME);
    client.print(',');
    client.print(h, 2);
    client.print(',');
    client.println(f, 2);

    Serial.println("closing connection");
    client.stop();

    Serial.println("wait 5 sec...");
    delay(5000);
}
