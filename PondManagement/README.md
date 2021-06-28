# Pond Management
This is a project for management of a medium-sized garden pond.

Required components:
3D printed enclosure (4 parts)
3D printed float sensor brackets (2 each, 4 parts total)
US #2 screws (or equivalent)
US #4 screws (or equivalent) with nuts
Grounded extension cord, 6 foot with two outlets (or long enough to reach power)
Grounded extension cord, 3 foot with single outlet
120VAC submersible pump
24VAC adapter (commonly used for irrigation)
24VAC normally-closed solenoid valve
2 float sensors (Hall effect)
Screen material
Small piece of clear glass
Hot glue gun
24AWG hookup wire, various colors
20AWG hookup wire, stranded
16AWG hookup wire, black
Dupont connector crimp kit (female for pi and for relay board, male for Arduino)
1 Arduino Uno board
1 Raspberry Pi 2B or later (for non-wifi Pi models, USB wifi adapter needed)
1 5V 3A power supply (any connector, will get hardwired in)
1 Raspberry Pi camera board
1 DHT11 temperature / humidity module
2 10k ohm resistors (for float sensors)
1 2-position or 4-position relay module meant for use with Arduino, such as the SainSmart module
1 24VDC fan
4 diodes (insert detail)
1 capacitor (insert detail)
1 small piece of perfboard
1 outlet and low-profile receptacle box
1 faceplate for outlet
Short USB A-B cable (to connect arduino to Pi)
Longer USB A-B cable (to connect arduino to computer for programming)

Software components (on computer):
Arduino IDE (Linux, Windows or Mac)
Raspbian OS

Software components (on raspbian OS aka Linux):
sudo apt-get update
sudo apt-get install build-essential python-dev
git clone https://github.com/adafruit/Adafruit_Python_DHT.git
cd Adafruit_Python_DHT
sudo python3 setup.py install
cd ..
sudo apt-get install python3-picamera
sudo apt-get install gpac

Design notes
The single-relay modules can be used with a Raspberry Pi at 3.3V switching level
as long as you use exactly one. The 2-relay modules and 4-relay modules will not
work because of the greater current levels needed.

Instead of using power transistors or MOSFETs to drive the solenoid relay modules,
I opted for a firmware approach where the Arduino firmware would handle the basic
"fill" and "drain" operations and monitor the float sensors. The Arduino could also
handle analog inputs (light sensors, thermistors, etc) but this is just for basic water
management and having some remote camera access.

The 5V DC power supply should be capable of a solid 3.5A as it will be powering the
Raspberry Pi, the Arduino, and the relay board. Note that although the Arduino can take about 8-12 VDC
input and convert it to 5V via an internal converter, some of the cheaper Arduino clones (such as Elegoo)
CANNOT drive the 4-relay module via 12V conversion, and either need to be powered via USB or
directly via the UNREGULATED 5V pin on the Arduino header. Note that this is not a recommended
approach but works perfectly well provided you have an actual switching power supply. This is not
a problem as they are available for around $12.

Using a fan for the enclosure is important to prevent heat buildup. It is also important to 
have structural shade as the enclosure is not meant to withstand direct sun or Texas thunderstorms.
It will protect the insides against incidental sprinkling, the occasional horizontal rain, etc.

Using the 24VAC adapter needed for valves to make a simple full-wave rectifier for the enclosure
fan is purely a decision driven by cheapness and a desire for efficiency. The 24VDC coming out
of the rectifier circuit is not that smooth but definitely adequate for the fan.
