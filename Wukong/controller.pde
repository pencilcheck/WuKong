#include <pt.h>

/*
  WuKong Server
  Author: Penn Su 
 */

#include <WuKong.h>

RequestHandler* handler;
Arduino* board;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  handler = new RequestHandler(new Arduino());
  board = handler->board();
  
  Serial.println("Starting up");
}

void loop() {
  //while (1) {
    // Read data from all pins on the board
    //board->readAll();

    // An example of retrieving a value from a pin
    //Serial.println(board->getDigitalSensors()[2]->read());

    // Receive command and respond to the request
    if (Serial.available())
      handler->handle(handler->receiveRequest());

    // Other subroutines here
    
    // Time-driven virtual sensor tasks for the board
    board->step();
  //}
}

