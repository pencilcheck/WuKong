 #include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t payload[] = {0, 0, 0, 0};

// SH + SL of XBee coordinator
//XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x407734C4);
// Need both 0x0 to broadcast to coordinator
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0x0);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

char location[] = "indoor";
int registered = 0;
int id = 0;
int i;

int inIRP0 = A0;

void setup() {
  pinMode(inIRP0, INPUT);
  
  xbee.begin(9600);
  Serial.begin(9600);
  
  randomSeed(analogRead(0));
  id = random(10);
}

void loop() {
  
  if (!registered) {
    //DEBUG
    //Serial.println("not registered");
    payload[0] = 'b';
    payload[1] = id;
  }
  else {
    //DEBUG
    //Serial.println("registered");
    
    int inIRV0 = digitalRead(inIRP0);
    int inIRV1 = digitalRead(inIRP1);
    int inIRV2 = digitalRead(inIRP2);
    int inIRV3 = digitalRead(inIRP3);
    int inIRV4 = digitalRead(inIRP4);
    int inIRV5 = digitalRead(inIRP5);
  
    payload[0] = 'd';
    payload[1] = id;
    payload[2] = 6;
    payload[3] = inIRV0;
    payload[4] = inIRV1;
    payload[5] = inIRV2;
    payload[6] = inIRV3;
    payload[7] = inIRV4;
    payload[8] = inIRV5;
  }
  
  xbee.send(zbTx);
  //DEBUG
  //Serial.println();
  
  xbee.readPacket(20);
  
  if (xbee.getResponse().isAvailable()) {
    
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);
      
      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        //Serial.println("ACK sent");
      }
      
      // RESET
      if (rx.getData(0) == 's') {
        registered = 0;
        id = random(10);
      }
      else if (rx.getData(0) == 'r') {
        //DEBUG
        //Serial.println("receiving reply");
        
        if (id == rx.getData(1)) {
          //DEBUG
          //Serial.println("matched id");  
          registered = 1;
          id = rx.getData(2);
        }
        
        if (rx.getData(3) == 'q') {
          if (rx.getData(4) == 'l') {
            payload[0] = 'l';
            payload[1] = id;
            payload[2] = sizeof(location);
            // Assuming location has shorter length than payload length - 3
            for (i = 0; i < sizeof(location); ++i) {
              payload[i+3] = location[i];
            }
            payload[3+sizeof(location)] = '\0';
            
            xbee.send(zbTx);
          }
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
}
