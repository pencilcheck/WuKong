#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

void setup()
{
  xbee.begin(9600);
  Serial.begin(9600);
}

void loop()
{
  /*
  while(Serial.available()) { 
      char getData = Serial.read();
      Serial.print(getData);
  }
  */
  
  xbee.readPacket();
  
  if(xbee.getResponse().isAvailable()) {
    
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);
      /*
      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        
      } else {
      
      }
      */
      // Received packet in decimal
      Serial.print(rx.getData(0), DEC);
      Serial.print(",");
      Serial.print(rx.getData(1), DEC);
      Serial.print(",");
      Serial.print(rx.getData(2), DEC);
      Serial.print(",");
      Serial.print(rx.getData(3), DEC);
      Serial.print(",");
      Serial.print(rx.getData(4), DEC);
      Serial.print(",");
      Serial.print(rx.getData(5), DEC);
      Serial.println();
    } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
      //xbee.getResponse().getModemStatusResponse(msr);
      Serial.println("modem received above");  
    } else if (xbee.getResponse().isError()) {
      Serial.print("error occurred:");
      if (xbee.getResponse().getErrorCode() == 1) {
        Serial.println("Checksum failure");
      }
      else if (xbee.getResponse().getErrorCode() == 2) {
        Serial.println("Packet Exceeds Byte Array Length");
      }
      else if (xbee.getResponse().getErrorCode() == 3) {
        Serial.println("Unexpected start byte");
      }
    }
  }
}
