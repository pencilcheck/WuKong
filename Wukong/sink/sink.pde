#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t payload[] = {0, 0, 0, 0, 0, 0};

// SH + SL, but 0x0 and 0xFFFF is for broadcast
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0xFFFF);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int i;
int command = 0;
int maxId = 5;
int registeredIds = 0;
int ids[] = {-1, -1, -1, -1, -1};

int repliedIds = 0;
int callbacks[] = {-1, -1, -1, -1, -1};

void addCallbackId(int callbackId) {
  callbacks[repliedIds] = callbackId;
  repliedIds++;
}

int replied(int callbackId) {
  for (i = 0; i < maxId; ++i) {
    if (callbackId == callbacks[i]) {
      return true;
    }
  }
  return false;
}

int newRegisterId(int callbackId) {
  int newId = callbackId;
  while (registered(newId)) {
    newId = random(10);
  }
  
  ids[registeredIds] = newId;
  if (registeredIds < maxId) {
    registeredIds++;
  }
  
  return newId;
}

int registered(int id) {
  for (i = 0; i < maxId; ++i) {
    if (id == ids[i]) {
      return true;
    }
  }
  return false;
}

void setup()
{
  xbee.begin(9600);
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop()
{
  /*
  if (Serial.available() > 0) {
    command = Serial.read();
    Serial.print("command received: ");
    Serial.println(command);
  }
  */
  
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable()) {
    
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);
      
      if (rx.getData(0) == 'b') {
        Serial.println("in b");
        
        int callbackId = rx.getData(1);
        
        if (replied(callbackId)) {
          //TODO: multiple callbackid message from the same node
        }
        else {
          addCallbackId(callbackId);
          
          int newId = newRegisterId(callbackId);
          
          payload[0] = 'r';
          payload[1] = callbackId;
          payload[2] = newId;
          
          xbee.send(zbTx);
          
          Serial.println("sent r");
        }
      }
      else if (rx.getData(0) == 'd') {
        Serial.println("in d");
        
        int id = rx.getData(1);
        
        if (registered(id)) {
          int length = rx.getData(2);
          for (i = 3; i < length; ++i) {
            Serial.print(rx.getData(i));
          }
          Serial.println();
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
