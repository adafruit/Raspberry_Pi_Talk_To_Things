# Raspberry Pi Talking To WiFi 'Things' Part 3

Use a push button connected to an ESP8266 to toggle a LED on a Raspberry Pi
on and off, and a push button connected to the Pi to toggle a LED on the ESP8266
on and off.  Shows how to use a Python socket server to listen for simple commands
over a socket, and how to send commands to an ESP8266 that's also listening over
a socket for commands.

You'll need the following hardware setup:

-   Raspberry Pi (any model).

-   LED connected to pin 18 of the Pi.  Connect the anode (longer leg) to pin 18
    and the cathode (shorter leg) **through a ~560 ohm resistor** to Pi ground.

-   A momentary pushbutton connected to pin 23 of the Pi.  Connect one corner of
    the pushbutton to Pi pin 23 and the opposite corner to Pi ground.

-   ESP8266 board.

-   LED connected to pin 2 of the ESP8266.  Connect the anode (longer leg) to
    pin 2 of the ESP8266 and the cathode (shorter leg) **through a ~560 ohm
    resistor** to ESP8266 ground.

-   A momentary pushbutton connected to pin 0 of the ESP8266.  Connect one
    corner of the pushbutton to pin 0 of the ESP8266 and the opposite corner
    to ESP8266 ground.

## Pi Setup

Because this code is written to use Python 3 you'll need to do a little setup
on the Pi to install Python 3 and dependencies.  Run the following commands on
the Pi:

    sudo apt-get update
    sudo apt-get install -y python3 python3-pip
    sudo pip3 install RPi.GPIO

In addition you will likely want to assign the Pi a static IP address so that
the ESP8266 can easily find it.  See part 1 of this series for information
on setting a static IP on the Pi.

# ESP8266 Setup

Load the ESP8266_LED_Button sketch in the Arduino IDE, modify the configuration
define values near the top for your WiFi network, then upload to the ESP8266
board.

## Usage

To run the Python server code navigate to the Python3_Server folder and run:

    python3 server.py

When you press the button on the Pi it will attempt to connect to the ESP8266
and issue a toggle_led command that turns the LED on/off.  Likewise when you
press the button on the ESP8266 it will attempt to connect to the Pi and issue
a toggle_led command that turns its LED on/off.
