#include <XBee.h>

XBee xbee = XBee();
ZBRxResponse rx = ZBRxResponse();

void setup()
{
  xbee.begin(9600);
  Serial.begin(9600);
}

void loop()
{ 
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable()) {
    
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);
      
      Serial.print(rx.getData(0));
      Serial.print(",");
      Serial.println(rx.getData(1));
    } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
      Serial.println("Modem received");
    } else if (xbee.getResponse().isError()) {
      if (xbee.getResponse().getErrorCode() == 1) Serial.println("Checksum failure");
      else if (xbee.getResponse().getErrorCode() == 2) Serial.println("Packet Exceeds Byte Array Length");
      else if (xbee.getResponse().getErrorCode() == 3) Serial.println("Unexpected start byte");
    }
  }
}
