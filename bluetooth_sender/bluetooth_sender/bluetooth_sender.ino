#include <SoftwareSerial.h>

#define RX 0
#define TX 1
SoftwareSerial mySerial (RX, TX);  //RX, TX (Switched on the Bluetooth - RX -> TX | TX -> RX)


void setup() {
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);
  
  // put your setup code here, to run once:
  mySerial.begin(9600);
  Serial.begin(9600);
}

void loop() {

  mySerial.println("a");
  mySerial.flush();
  
  Serial.println("loop");
  delay (200); //prepare for data (2s)

}
