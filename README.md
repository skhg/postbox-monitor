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

## Design, build & test
My postbox is a sealed metal box, one of a bank of other postboxes installed in a public hallway in my building. This severely constrains the options for remote monitoring.
* Solid metal means WiFi can't penetrate.
* No external power supply is possible.
* No interference with my neighbours postboxes is allowed.
* Nothing can be mounted on the outside, so as not to attract attention.

On the first point however there is a tiny exception - the only gap in the metal box is a slot for a plastic name tag, just a few cm wide. But it's big enough to let a signal through!

So next consideration is how to make this as low-maintenance as possible. Just do the absolute minimum, use as little power as possible, and keep operating even if external systems go offline.

### Extreme power efficiency
I chose the [FireBeetle ESP32](https://www.dfrobot.com/product-1590.html) board due to its extreme power efficiency. It supports a deep sleep mode that consumes just 10μA, with no board modifications required! Before sleeping, a number of trigger pins can be set. One can wake the board when the flap opens, and another when the door opens.

### Resilient WiFi connectivity
The WiFi network from my apartment upstairs might not always be available, due to various factors. So it's important the board can make a best effort to connect, but not waste time if the network or endpoint is unavailable at that moment.

I use a simple backoff strategy which doubles the time interval between subsequent retries, up to a maximum of 24 hours. Every 24 hours the system pings home to report it's still alive, so there is no need to exceed this limit.

### Circuit diagram
Here's a general overview:
![Circuit overview](images/postbox-monitor_bb.png "Circuit overview")
<i>(Exported from Fritzing [file](images/postbox-monitor.fzz))</i>

### Workflow


## Installation
Here's how it was designed for installation with a bus board:
![Installation diagram](images/real-installation_bb.png "Installation diagram")
<i>(Exported from Fritzing [file](images/real-installation.fzz))</i>

## Event handling and monitoring
I run a Spring Boot service which hosts several controllers for capturing domestic data (e.g. my [weather station](https://github.com/skhg/weather-station)). This also includes two endpoints for capturing events from the postbox:

`POST /postbox/boot`

`POST /postbox/delivered`

`POST /postbox/retrieved`

`POST /postbox/ping`

Each call also supplies a very minimal JSON blob, for example:

```json
{
  "deviceVoltageMeasurement": 1234,
  "retryCount": 5
}
```

When my service handles a `/delivered` or `/retrieved` request, I either send a new email notifying that post has arrived, or clear the message. The details of how that's done are outside the scope of this project.  

With the additional device data I recieve in the JSON contents, I can conveniently monitor the power voltage (in mV) and the condition of the WiFi connection over time.

## Performance analysis
My service pushes all event data to InfluxDB for later analysis using Grafana. After a few hiccups at the start the system completed it's commissioning on DATE.

## Ideas for improvement

