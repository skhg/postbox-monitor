#include <SoftwareSerial.h>

#include <LowPower.h>

#define post_flap 2
#define door_flap 3

#define post_interrupt 0
#define door_interrupt 1

#define RX 0
#define TX 1

#define BT_SWITCH A0

SoftwareSerial bluetooth (RX, TX);  //RX, TX (Switched on the Bluetooth - RX -> TX | TX -> RX)

void setup() {
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);

  pinMode(BT_SWITCH, OUTPUT);
  
  pinMode(post_flap, INPUT);
  pinMode(door_flap, INPUT);

  bluetooth.begin(9600);
  Serial.begin(9600);
}

void wake() {
}

void safeTransmit(String line){
  bluetooth.println(line);

  bluetooth.flush();

  Serial.println(line);
  
  while (!(UCSR0A & (1 << UDRE0)))  // Wait for empty transmit buffer
    UCSR0A |= 1 << TXC0;  // mark transmission not complete
  while (!(UCSR0A & (1 << TXC0)));   // Wait for the transmission to complete
}

void interruptSleep(int interruptId){
  attachInterrupt(interruptId, wake, LOW);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  
  // Disable external pin interrupt on wake up pin.
  detachInterrupt(interruptId); 
}

void bluetoothPower(bool on){
  if(on){
    digitalWrite(BT_SWITCH, HIGH);
  }else{
    digitalWrite(BT_SWITCH, LOW);
  }
}

void loop() {

    interruptSleep(post_interrupt);
    bluetoothPower(true);

    delay(10000);
    //TODO: turn on bluetooth here, wait a bit for activation
    safeTransmit("p");
    delay(1000);
    //TODO: turn off bluetooth here
    //TODO: when turning off bluetooth, check if the TX/RX/GND pins also have to be manually disabled somehow
    bluetoothPower(false);
  


    
    interruptSleep(door_interrupt);
    bluetoothPower(true);
    delay(10000);


    //TODO: turn on bluetooth here, wait a bit for activation
    safeTransmit("r");
    delay(500); //todo: check how reliable this delay is
    safeTransmit("r");
    delay(1000);
    //TODO: turn off bluetooth here
    bluetoothPower(false);
}
