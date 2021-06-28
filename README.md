# greenhouse-monitor
Greenhouse monitor and control using Arduino, and other garden-type electronics for monitoring and control

I've been running this greenhouse monitor (in various forms) for the last 7 years or so. Hardware consists of an Arduino Uno board with an Ethernet shield, connected to a 4-relay board with 2 10-amp relay switches used for 800-watt heaters running on 120VAC and the other two optionally used to power 24-volt sprinkler valve solenoids.
Sensor input is from an I2C temperature / humidity sensor and (optional) thermistor wires.
A more recent configuration is in a shed with no outside sunlight where the one of the 120VAC lines is used to power LED grow lights.

Also in this repo is the more recent pond management system.

Setup:
Add libraries/DHT via Arduino IDE (dht11 required for I2C temperature and humidity sensor)

