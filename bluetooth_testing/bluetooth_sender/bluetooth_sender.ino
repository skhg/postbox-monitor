#include <SoftwareSerial.h>

#define RX 0
#define TX 1
SoftwareSerial mySerial (RX, TX);  //RX, TX (Switched on the Bluetooth - RX -> TX | TX -> RX)

#define bluetoothSwitch A0

void setup() {
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);

  pinMode(bluetoothSwitch, OUTPUT);


  digitalWrite(bluetoothSwitch, HIGH);
  
  // put your setup code here, to run once:
  mySerial.begin(9600);
  Serial.begin(9600);
}

int counter = 0;
bool on = false;

void loop() {

//  counter++;
//  if(counter == 100){
//    counter = 0;
//
//    on = !on;
//
//    if(on){
//      Serial.println("on");
//      digitalWrite(bluetoothSwitch, HIGH);
//      delay(20000);
//    }else{
//      Serial.println("off");
//      digitalWrite(bluetoothSwitch, LOW);
//    }
//  }
//
//  
//  
//  
//  mySerial.println("a");
//  mySerial.flush();
//
//  if(on){
//    Serial.println("send");
//  }
//  
//  delay (200); //prepare for data (2s)

}
