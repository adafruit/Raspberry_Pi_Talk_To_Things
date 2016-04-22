# Raspberry Pi Talking To WiFi 'Things' Part 2

See the Arduino sketch for this video in the ESP8266_LED_Control folder.  This
sketch will configure an ESP8266 board to turn on & off a LED connected to pin
2 using simple commands sent from a Raspberry Pi & netcat.  In addition the
ESP8266 will use a multicast DNS responder so it can be discovered under the
name 'esp8266-thing.local'.

Once the sketch is running you can send it commands from a Pi using netcat, like:

    echo 'ON' | nc esp8266-thing.local 5000

To turn the LED on.  To turn off the LED instead use:

    echo 'OFF' | nc esp8266-thing.local 5000
