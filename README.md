# postbox-monitor
Arduino installation to detect when post is delivered to our postbox

## Raspberry Pi pairing

https://www.raspberrypi.org/forums/viewtopic.php?t=163712 will get it working with no additional system config changes

Magic is `sudo rfcomm connect hci0 AA:BB:CC:DD:EE:FF`
or maybe `sudo rfcomm listen hci0 AA:BB:CC:DD:EE:FF`


## Send data from Arduino

http://blog.whatgeek.com.pt/2015/09/bluetooth-communication-between-raspberry-pi-and-arduino/

## Trigger BT connection from Arduino
Maybe?
https://stackoverflow.com/questions/18752499/initiate-connection-from-arduino-to-remote-device


Required on Raspi

`sudo apt-get install bluez python-bluez`

## Low Power

http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/

## Transistor

https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-resistor-color-code-5-band
https://learn.adafruit.com/transistors-101?view=all


## SMS gateway
https://www.opentextingonline.com/emailtotext.aspx

## Send emails
https://stackabuse.com/how-to-send-emails-with-gmail-using-python/

## Sensor
https://randomnerdtutorials.com/monitor-your-door-using-magnetic-reed-switch-and-arduino/
