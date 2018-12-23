# greenhouse-monitor
Greenhouse monitor and control using Arduino.
I've been running this for the last 5 years or so. Hardware consists of an Arduino Uno board with an Ethernet shield, connected to a 4-relay board with 2 10-amp relay switches used for 800-watt heaters running on 120VAC and the other two optionally used to power 24-volt sprinkler valve solenoids.
Sensor input is from an I2C temperature / humidity sensor and (optional) thermistor wires.
A more recent configuration is in a shed with no outside sunlight where the one of the 120VAC lines is used to power LED grow lights.

Setup:
Add libraries/DHT via Arduino IDE (dht11 required for I2C temperature and humidity sensor)

