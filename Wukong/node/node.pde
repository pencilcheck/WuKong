#include <XBee.h>

XBee xbee = XBee();

uint8_t payload[] = {0, 0, 0, 0, 0, 0};

// SH + SL of XBee coordinator
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x407734C4);
//XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int outMotionPin = A0;
int outIR1Pin = A1;
int outIR2Pin = A2;
int inMotionPin = A3;
int inIR1Pin = A4;
int inIR2Pin = A5;

void setup() {
  pinMode(outMotionPin, INPUT);
  pinMode(inMotionPin, INPUT);
  //Serial.begin(9600);
  xbee.begin(9600);
}

void loop() {  
  
  int outMotion = digitalRead(outMotionPin);
  int inMotion = digitalRead(inMotionPin);
  
  // Use XBee library to transmit data
  float voltOutIR1 = analogRead(outIR1Pin) * 0.0049;
  float voltOutIR2 = analogRead(outIR2Pin) * 0.0049;
  float voltInIR1 = analogRead(inIR1Pin) * 0.0049;
  float voltInIR2 = analogRead(inIR2Pin) * 0.0049;
 
  // used wolfram alpha and got the inverse function for the graph
  int distOutIR1 = 187.828 * pow(2.718281828, -0.917212 * voltOutIR1);
  int distOutIR2 = 187.828 * pow(2.718281828, -0.917212 * voltOutIR2);
  int distInIR1 = 187.828 * pow(2.718281828, -0.917212 * voltInIR1);
  int distInIR2 = 187.828 * pow(2.718281828, -0.917212 * voltInIR2);
  
  // some blogger's function
  //int distOutIR1 = 65 * pow(voltOutIR1, -1.10);
  //int distOutIR2 = 65 * pow(voltOutIR2, -1.10);
  //int distInIR1 = 65 * pow(voltInIR1, -1.10);
  //int distInIR2 = 65 * pow(voltInIR2, -1.10);

  payload[0] = outMotion;
  payload[1] = distOutIR1;
  payload[2] = distOutIR2;
  payload[3] = inMotion;
  payload[4] = distInIR1;
  payload[5] = distInIR2;

  // debugging
  //Serial.println(outMotion);
  /*
  Serial.print(digitalRead(outMotionPin));
  Serial.print(",");
  Serial.print(distOutIR1);  
  Serial.print(",");
  Serial.print(distOutIR2);  
  Serial.print(",");
  Serial.print(digitalRead(inMotionPin));  
  Serial.print(",");
  Serial.print(distInIR1);  
  Serial.print(",");
  Serial.print(distInIR2);  
  Serial.println();
  */
  
  xbee.send(zbTx);
  delay(10);
}
