# Raspberry Pi Talking To WiFi 'Things' Part 4

Use a push button connected to an ESP8266 to toggle a LED on a Raspberry Pi
on and off, and a push button connected to the Pi to toggle a LED on the ESP8266
on and off.  This version uses MQTT and a MQTT broker running on the Raspberry Pi
to send and receive LED toggle messages between devices.

You'll need the same hardware setup as in Part 3 (see the Part 3 README.md).

## Pi Setup

Because this code is written to use Python 3 you'll need to do a little setup
on the Pi to install Python 3 and dependencies.  Run the following commands on
the Pi:

    sudo apt-get update
    sudo apt-get install -y python3 python3-pip
    sudo pip3 install RPi.GPIO paho-mqtt

Next you will need to install the mosquitto MQTT broker.  You can follow the
instructions here to add the mosquitto broker apt repository to the Pi and install
mosquitto (make sure to use the mosquitto-jessie.list file for the current latest
version of Raspbian): http://mosquitto.org/2013/01/mosquitto-debian-repository/

By default after installing mosquitto it will automatically run and not require
any authentication for MQTT users.  Be aware this is not necessarily insecure
but other users could connect to your Pi and control your devices!  

In addition you will likely want to assign the Pi a static IP address so that
the ESP8266 can easily find it.  See part 1 of this series for information
on setting a static IP on the Pi.

# ESP8266 Setup

You will need to install the following Arduino libraries to use the sketch for
this example (use the library manager to easily install them):

-   [Adafruit MQTT](https://github.com/adafruit/Adafruit_MQTT_Library)

-   [Adafruit SleepyDog](https://github.com/adafruit/Adafruit_SleepyDog)

Load the ESP8266_LED_Button_MQTT sketch in the Arduino IDE, modify the configuration
define values near the top for your WiFi network, then upload to the ESP8266
board.

## Usage

To run the Python code navigate to the Python_LED_Button_MQTT folder and run:

    python3 main.py

When you press the button on the Pi it will attempt to connect to the ESP8266
and issue a toggle led command that turns the LED on/off.  Likewise when you
press the button on the ESP8266 it will send a toggle LED message to the Pi.
