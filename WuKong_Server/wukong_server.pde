/*
  WuKong Server
  Author: Penn Su 
 */

#include <WuKong.h>

RequestHandler* handler;
Arduino* board;

void setup() {
  handler = new RequestHandler(new InfraredUno());
  board = handler->board();
  
  Serial.begin(9600);
  Serial.println("Welcome to WuKong, enjoy your stay!");
}

void loop() {
  Serial.println("entering loop()");
  while (1) {
    // Read all data from all pins on the board
    handler->readAll();

    // An example of retrieving a value from a pin
    Serial.println(board->getDigitalSensors()[2]->val());

    handler->oneStep(); // Calling callbacks
    
    // Receive command and respond to the request
    if (Serial.available())
      handler->handle(handler->receiveRequest());
    
    // Other subroutines here
    
    handler->rateControl(); // Delay specified by the requirements from the client
  }
}
