# Raspberry Pi Talking To WiFi Things Part 4
# Use a push button connected to an ESP8266 to toggle a LED on a Raspberry Pi
# on and off, and a push button connected to the Pi to toggle a LED on the ESP8266
# on and off.  This version uses MQTT to send and receive LED control messages.
# Author: Tony DiCola
import time

import RPi.GPIO as GPIO
import paho.mqtt.client as mqtt


# Configuration:
LED_PIN        = 18
BUTTON_PIN     = 23


# Initialize GPIO for LED and button.
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.output(LED_PIN, GPIO.LOW)
GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Setup callback functions that are called when MQTT events happen like
# connecting to the server or receiving data from a subscribed feed.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("/leds/pi")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print('{0} got: {1}'.format(msg.topic, msg.payload))
    # Check if this is a message for the Pi LED.
    if msg.topic == '/leds/pi':
        # Look at the message data and perform the appropriate action.
        if msg.payload == b'ON':
            GPIO.output(LED_PIN, GPIO.HIGH)
        elif msg.payload == b'OFF':
            GPIO.output(LED_PIN, GPIO.LOW)
        elif msg.payload == b'TOGGLE':
            GPIO.output(LED_PIN, not GPIO.input(LED_PIN))

# Create MQTT client and connect to localhost, i.e. the Raspberry Pi running
# this script and the MQTT server.
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect('localhost', 1883, 60)

# Connect to the MQTT server and process messages in a background thread.
client.loop_start()

# Main loop to listen for button presses.
print('Script is running, press Ctrl-C to quit...')
while True:
    # Look for a change from high to low value on the button input to
    # signal a button press.
    button_first = GPIO.input(BUTTON_PIN)
    time.sleep(0.02)  # Delay for about 20 milliseconds to debounce.
    button_second = GPIO.input(BUTTON_PIN)
    if button_first == GPIO.HIGH and button_second == GPIO.LOW:
        print('Button pressed!')
        # Send a toggle message to the ESP8266 LED topic.
        client.publish('/leds/esp8266', 'TOGGLE')
