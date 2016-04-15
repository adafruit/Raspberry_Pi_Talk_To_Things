# Raspberry Pi Talking To WiFi 'Things' Part 1

See the Arduino sketch for this video in the ESP8266_DHT_Sensor folder.

You can run a netcat server to log data with a command like this on your pi:

    nc -l -k 5000

You might want to make it more advanced by adding a timestamp and comma to
the front of the logged data.  First install the moreutils packages with:

    sudo apt-get install moreutils

Then use the ts program to add a timestamp:

    nc -l -k 5000 | ts "%b %d %H:%M:%S,"

Or even save it to a file sensor_data.txt by running:

    nc -l -k 5000 | ts "%b %d %H:%M:%S," >> sensor_data.txt
