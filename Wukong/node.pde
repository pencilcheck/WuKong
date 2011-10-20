#include <XBee.h>

XBee xbee = XBee();

uint8_t payload[] = {0, 0, 0};

// SH + SL of XBee coordinator
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x407734C4);
//XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int leftPin = 8;
int rightPin = 4;
int doorPin = 3;

void setup() {
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(doorPin, INPUT);
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
  payload[0] = digitalRead(leftPin);
  payload[1] = digitalRead(rightPin);
  payload[2] = digitalRead(doorPin);
  
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
