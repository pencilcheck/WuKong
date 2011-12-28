#include <XBee.h>

class Profile {
  public:
  Profile();
  
  private:
  int type;
};

class Node {
  public:
  Node()
  : numProfiles(0) {
  };
  
  int addProfile(Profile* p) {
    if (numProfiles < 10) {
      profiles[numProfiles++] = p;
      return numProfiles-1;
    }
    else {
      return -1;
    }
  };
  
  Profile* removeProfile(int index) {
    Profile* p = profiles[index];
    profiles[index] = 0;
    for (int i = index; i < 9; ++i) {
      profiles[i] = profiles[i+1];
    }
    profiles[9] = 0;
    return p;
  };
  
  private:
  int numProfiles;
  Profile* profiles[10];
};

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t payload[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// SH + SL of XBee coordinator
//XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x407734C4);
// Need both 0x0 to broadcast to coordinator
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0x0);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

char location[] = "outdoor";
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
  
  if (!registered) {
    payload[0] = 'b';
    payload[1] = id;
  }
  else {
    int inIRV0 = digitalRead(inIRP0);
    int inIRV1 = digitalRead(inIRP1);
    int inIRV2 = digitalRead(inIRP2);
    /*int inIRV1 = analogRead(inIRP1) * 0.0049;
    int inIRV2 = analogRead(inIRP2) * 0.0049;
    int inIRV3 = analogRead(inIRP3) * 0.0049;
    int inIRV4 = analogRead(inIRP4) * 0.0049;*/
    int inMV5 = digitalRead(inMP5);
    
    /*int inDist1 = 187.828 * pow(2.718281828, -0.917212 * inIRV1);
    int inDist2 = 187.828 * pow(2.718281828, -0.917212 * inIRV2);
    int inDist3 = 187.828 * pow(2.718281828, -0.917212 * inIRV3);
    int inDist4 = 187.828 * pow(2.718281828, -0.917212 * inIRV4);*/
  
    payload[0] = 'd';
    payload[1] = id;
    payload[2] = 4;
    payload[3] = inIRV0;
    payload[4] = inIRV1;
    payload[5] = inIRV2;
    payload[6] = inMV5;
  }
  
  
  xbee.send(zbTx);

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
        
        if (id == rx.getData(1)) {
          
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
