 #include <XBee.h>

XBee xbee = XBee();
ZBRxResponse rx = ZBRxResponse();

uint8_t payload[] = {0, 0, 0};

// SH + SL of XBee coordinator
// Need both 0x0 to broadcast to coordinator
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0x0);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

float distance(float volt) {
  float intermediate = volt * 0.0049;
  return 187.828 * pow(2.718281828, -0.917212 * intermediate);
}

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  
  xbee.begin(9600);
  Serial.begin(9600);
}

void loop() {
  payload[0] = digitalRead(A0);
  payload[1] = digitalRead(A1);
  payload[2] = (int)distance(analogRead(A2));
  
  xbee.send(zbTx);
  delay(20);
}
