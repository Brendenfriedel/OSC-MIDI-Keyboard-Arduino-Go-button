# OSC MIDI Keyboard Arduino Go button
Code for an Arduino Due that allows you to send OSC, MIDI, or Keyboard commands to a computer. To be used mainly with QLab.
Devloped by Brenden Friedel for use with Arduino Due Microcontroller

### Features 
  * Webserver to allow network setting changes 
  * Built-in menu system that allows you to change modes, reset, and view current statistics
  * Choice to send OSC, MIDI, or Keyboard commands to a computer
  * Saves Network settings localy on SD Card 
 
### Needed Materials
   * Arduino Due Micro-contoller
   * Arduino Network Shield with SD Card Slot
   * 4 LED Buttons
   * I2C Liquid Crystal Display
   * SD Card for writing information
### Useful info
In my current setup, I am using 4 buttons and a display. The top button is Red, The right button is Blue, the left button is Yellow, and the bottom button is Green
* Default Network information 
  * IP Address: 192.168.1.5
  * Subnet Address: 255.255.255.0
  * Destination IP Address: 192.168.1.50
  * Destination Port: 53000
  * Receive Port: 53069 (This value cannot be changed on webserver)
  * Ethernet Server Port: 80 
* Pressing Left and Right buttons will restore network settings to default.
  * Button and LED Pins
    *Go Button/LED: 30/42
    *Stop Button/LED: 31/41
    *Previous Button/LED: 32/43
    *Next Button/LED: 33/40
    *I2C Diaply wired to SDA and SCL Pins
    *Pins 4,10,11,12,13 are reserved for ethernet shield communication
