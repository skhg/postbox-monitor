#include <SoftwareSerial.h>

#include <LowPower.h>

#define post_flap 2
#define door_flap 3

#define post_interrupt 0
#define door_interrupt 1

void setup() {
  pinMode(post_flap, INPUT);
  pinMode(door_flap, INPUT);
  
  Serial.begin(9600);
}

void wake() {
}

void printlnWaitForTransmitFinish(String line){
  Serial.println(line);

  Serial.flush();

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

void loop() {

    interruptSleep(post_interrupt);

    printlnWaitForTransmitFinish("post arrived!");

    interruptSleep(door_interrupt);

    printlnWaitForTransmitFinish("you got your post");

}
