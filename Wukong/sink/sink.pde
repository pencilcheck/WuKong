#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t payload[] = {0, 0, 0, 0, 0, 0};

// SH + SL, but 0x0 and 0xFFFF is for broadcast to routers
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0xFFFF);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int i;
int command = 0;
int maxId = 5;
int registeredIds = 0;
int ids[] = {-1, -1, -1, -1, -1};
char* locations[5] = {0, 0, 0, 0, 0};

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
  
  // RESET
  payload[0] = 's';
  xbee.send(zbTx);
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
      
      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        //Serial.println("ACK sent");
      }

      // DEBUG
      //Serial.println((char)rx.getData(0));
      
      if (rx.getData(0) == 'b') {
        
        int callbackId = rx.getData(1);
        
        if (replied(callbackId)) {
          //TODO: multiple callbackid message from the same node
          Serial.print(callbackId);
          Serial.println(" already replied");
        }
        else {
          Serial.print("new callback id ");
          Serial.println(callbackId);
          
          addCallbackId(callbackId);
          
          int newId = newRegisterId(callbackId);
          
          payload[0] = 'r';
          payload[1] = callbackId;
          payload[2] = newId;
          payload[3] = 'q';
          payload[4] = 'l';
          
          xbee.send(zbTx);
        }
      }
      else if (rx.getData(0) == 'l') {
        Serial.println("got location response");
        int id = rx.getData(1);
        
        if (registered(id)) {
          
          char* location = (char*)malloc(sizeof(char) * (rx.getData(2)+1));
          for (i = 0; i < rx.getData(2); ++i) {
            location[i] = rx.getData(i+3);
          }
          location[rx.getData(2)] = '\0';
          Serial.print(id);
          Serial.print(" node is at ");
          Serial.println(location);
          for (i = 0; i < 5; ++i) {
            if (ids[i] == id) {
              break;
            }
          }
          locations[i] = location;
          Serial.println(locations[i]);
        }
      }
      else if (rx.getData(0) == 'd') {
        int id = rx.getData(1);
        
        if (registered(id)) {
          
          int length = rx.getData(2) + 3;

          Serial.print(id);
          Serial.print(":");
          
          int j;
          for (j = 0; j < 5; ++j) {
            if (ids[j] == id) {
              break;
            }
          }
          
          Serial.print(locations[j]);
          Serial.print(":");
          
          for (i = 3; i < length; ++i) {
            Serial.print(rx.getData(i));
            if (i < length-1) Serial.print(',');
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
