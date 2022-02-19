# postbox-monitor
I don't like checking my postbox every day for new deliveries, so I built a monitoring system that notifies me by email when something arrives. It works!

## The finished product
Here's how it looks, when installed:

![Inside the door](images/door-inside-angle.jpeg "Inside the door")

That's a battery pack, main bus board, microcontroller board, and two switches, all stuck on the inside of my postbox door. It looks like a mess, but the important thing is to ensure nothing falling into the postbox can dislodge any cables or connectors accidentally.

## Goals for the project
 * Be **virtually invisible** from outside the postbox
 * Run on battery power for at least ~1 year without discharging
 * Break "rarely"
 * "Very low" false positive rate 
 * Be resiliant to WiFi outages or failure

## Materials required
Electronics:
* 4x AAA battery pack 
* Various cables
* Perfboard
* JST connectors
* 1x FireBeetle ESP32 microcontroller
* Microswitches

Other:
* Little blocks of wood
* Thin pieces of rubber
* Lots of insulating tape
* Small screws

Other systems:
* Since this device can't send email by itself, you'll need something which can host the REST endpoints that this board will connect to, when events occur. More on that later 

<i>(Exported from Fritzing [file](traffic-light-board.fzz))</i>