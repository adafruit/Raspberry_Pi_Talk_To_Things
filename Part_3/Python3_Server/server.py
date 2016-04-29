# Raspberry Pi Talking To WiFi Things Part 3
# Use a push button connected to an ESP8266 to toggle a LED on a Raspberry Pi
# on and off, and a push button connected to the Pi to toggle a LED on the ESP8266
# on and off.  This Python socket server will listen for toggle LED commands
# from the ESP8266 which are sent when its button is pressed, and it will listen
# for button presses on the Pi and likewise send a toggle LED command to the
# ESP8266.
# Author: Tony DiCola
import atexit
import socket
from socketserver import *
import threading
import time

import RPi.GPIO as GPIO


# Configuration:
LED_PIN        = 18
BUTTON_PIN     = 23
SERVER_ADDRESS = '0.0.0.0'
SERVER_PORT    = 5000
ESP8266_NAME   = 'esp8266-thing.local'


# Class that the TCP socket server uses to handle connections.
class ServerHandler(StreamRequestHandler):

    def handle(self):
        # Handle function is called when a new connection comes in to the Pi.
        # Read a line of input from the socket.
        data = self.rfile.readline().strip()
        # Check if the toggle_led command was received.
        if data.lower() == b'toggle_led':
            # Received the toggle command, print out and toggle the LED state.
            print('Toggle LED!')
            GPIO.output(LED_PIN, not GPIO.input(LED_PIN))
        else:
            print('Unknown command: {0}'.format(data))


if __name__ == "__main__":
    # Initialize GPIO for LED and button.
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(LED_PIN, GPIO.OUT)
    GPIO.output(LED_PIN, GPIO.LOW)
    GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    # Create the server and run it in a background daemon thread.
    server = TCPServer((SERVER_ADDRESS, SERVER_PORT), ServerHandler)
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True
    server_thread.start()
    # Make sure the server gracefully shuts down at exit so the listening socket
    # isn't held open.
    atexit.register(server.shutdown)

    # Now run main loop in the main thread.  Listen for button presses
    # and send a toggle LED command to the esp8266 board.
    print('Server is running, press Ctrl-C to quit...')
    while True:
        # Look for a change from high to low value on the button input to
        # signal a button press.
        button_first = GPIO.input(BUTTON_PIN)
        time.sleep(0.02)  # Delay for about 20 milliseconds to debounce.
        button_second = GPIO.input(BUTTON_PIN)
        if button_first == GPIO.HIGH and button_second == GPIO.LOW:
            print('Button pressed!')
            # Connect to esp8266 board and send it a toggle_led command.
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as esp8266:
                try:
                    # Connect to the ESP8266 and then send a toggle_led command.
                    esp8266.connect((ESP8266_NAME, SERVER_PORT))
                    esp8266.sendall(b'toggle_led')
                    # Now the socket will be closed by the with context manager.
                except OSError:
                    # Something might have gone wrong connecting to the ESP8266
                    # socket. Print an error and move on to try again when the
                    # button is pressed.
                    print('Error connecting to ESP8266!')
