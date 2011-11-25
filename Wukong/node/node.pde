#include <XBee.h>

XBee xbee = XBee();

uint8_t payload[] = {0, 0, 0, 0, 0, 0};

// SH + SL of XBee coordinator
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x407734C4);
//XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int inMotionPin = 3;
int inIR1Pin = 4;
int inIR2Pin = 5;

int outMotionPin = 7;
int outIR1Pin = 9;
int outIR2Pin = 11;

void setup() {
  pinMode(outMotionPin, INPUT);
  pinMode(inMotionPin, INPUT);
  pinMode(outIR1Pin, INPUT);
  pinMode(outIR2Pin, INPUT);
  pinMode(inIR1Pin, INPUT);
  pinMode(inIR2Pin, INPUT);
  //Serial.begin(9600);
  xbee.begin(9600);
}

void loop() {
  //char msg[150];
  //sprintf(msg, "left %d,right %d,door %d", digitalRead(leftPin), digitalRead(rightPin), digitalRead(doorPin));
  //Serial.println(msg);
  /*
  // Write 3 bytes for each iteration left, right, and door
  Serial.print(digitalRead(leftPin));
  Serial.print(digitalRead(rightPin));
  Serial.print(digitalRead(doorPin));
  Serial.print('\n');
  delay(10);
  */
  
  // Use XBee library to transmit data
  payload[0] = digitalRead(outMotionPin);
  payload[1] = digitalRead(outIR1Pin);
  payload[2] = digitalRead(outIR2Pin);
  payload[3] = digitalRead(inMotionPin);
  payload[4] = digitalRead(inIR1Pin);
  payload[5] = digitalRead(inIR2Pin);
  
  xbee.send(zbTx);
  /*
  if (xbee.readPacket(10)) {
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      
      if (txStatus.getDeliveryStatus() == SUCCESS) {
      }
      else {
      }
    }
  } else if (xbee.getResponse().isError()) {
    
  }
  */
  //xbee.readPacket(1);
  delay(20);
  
}
