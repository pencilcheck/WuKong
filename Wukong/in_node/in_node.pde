#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t payload[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

// SH + SL of XBee coordinator
//XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x407734C4);
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0xFFFF);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int registered = 0;
int id = 0;
int i;

int inIRP0 = A0;
int inIRP1 = A1;
int inIRP2 = A2;
int inIRP3 = A3;
int inIRP4 = A4;
int inMP5 = A5;

void setup() {
  pinMode(inIRP0, INPUT);
  pinMode(inIRP1, INPUT);
  pinMode(inIRP2, INPUT);
  pinMode(inIRP3, INPUT);
  pinMode(inIRP4, INPUT);
  pinMode(inMP5, INPUT);
  
  // pullup resistor to draw less current to the motion sensor,
  // to prevent lower sensitivity for other sensors
  digitalWrite(inMP5, HIGH);
  
  //Serial.begin(9600);
  xbee.begin(9600);
  
  randomSeed(analogRead(0));
  id = random(10);
}

void loop() {

  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable()) {
    
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);
      
      if (rx.getData(0) == 'r') {
        if (id == rx.getData(1)) {
          id = rx.getData(2);
          registered = 1;
        }
      }
    } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
      xbee.getResponse().getModemStatusResponse(msr);
      Serial.println("Modem received");
    } else if (xbee.getResponse().isError()) {
      if (xbee.getResponse().getErrorCode() == 1) Serial.println("Checksum failure");
      else if (xbee.getResponse().getErrorCode() == 2) Serial.println("Packet Exceeds Byte Array Length");
      else if (xbee.getResponse().getErrorCode() == 3) Serial.println("Unexpected start byte");
    }
  }
  
  if (!registered) {
    payload[0] = 'b';
    payload[1] = id;
  }
  else {
    int inIRV0 = digitalRead(inIRP0);
    int inIRV1 = digitalRead(inIRP1);
    int inIRV2 = digitalRead(inIRP2);
    int inIRV3 = digitalRead(inIRP3);
    int inIRV4 = digitalRead(inIRP4);
    int inMV5 = digitalRead(inMP5);
  
    payload[0] = 'd';
    payload[1] = id;
    payload[2] = 6;
    payload[3] = inIRV0;
    payload[4] = inIRV1;
    payload[5] = inIRV2;
    payload[6] = inIRV3;
    payload[7] = inIRV4;
    payload[8] = inMV5;
  }
  /*
  // Received packet in decimal
  for (i = 0; i < sizeof(payload); ++i) {
    Serial.print(payload[i]);
  }
  Serial.println();
  */
  xbee.send(zbTx);
  delay(10);
}
