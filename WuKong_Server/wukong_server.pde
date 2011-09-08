/*
  People count in a room
  Need to know if there is people coming in or going out
  
  Need two IR sensors, one near the door and one two steps further inside
  both sensors are placed facing parallel to the door
  
  Need two states, one for each sensor
  
  For example, let sensor A be the sensor near the door and sensor B be the sensor two steps in.
  and let each state with corresponding letter bind to corresponding sensors
  
  If stateA is set before setting stateB, then we can infer that a person is comming in
  If stateB is set before setting stateA, then we can infer that a person is leaving
  
  Take the first derivative and only care about 0 to 1 for each sensor to set states
  
  IR sensor default working distance is 55cm
  Motion sensor default working distance is 
  
  Author: Penn Su 
 */

#include <WuKong.h>

#define TIMEOUT 1000

boolean stateA = false;
boolean stateB = false;
int count = 0;

RequestHandler* handler;

void setup() {
  handler = new RequestHandler();
  
  Serial.begin(9600);
  Serial.println("Welcome to WuKong, enjoy your stay!");
}

void loop() {
  Serial.println("entering loop()");
  while (1) {
    Serial.println("entering readAll()");
    // Read all data from all pins on the board
    handler->readAll();
    Serial.println("leaving readAll()");

    Serial.println("value of ditigal sensor at pin 2:");
    Serial.println(handler->board()->getDigitalSensors()[2]->val());
    
    // Receive command and respond to the request
    //if (Serial.available())
    //  handler->handle(handler->receiveRequest());
    
    // Like maybe send something back if some conditions are true?
    
    // Other subroutines
    
    
    
    //ArduinoUno* uno = new ArduinoUno();
    //uno->getDigitalSensors()[2]->read();
    //uno->getDigitalSensors()[3]->read();
    
    /*if (uno->getDigitalSensors()[2]->change() == -1) {
      Serial.println("Someone crossed the IR sensor at pin 2");
      if (stateB) {
        if (count > 0)
          count--;
        stateB = false;
        
        Serial.print("There are ");
        Serial.print(count);
        Serial.println(" in the room.");
      }
      else {
        stateA = true;
      }
    }
  
    if (uno->getDigitalSensors()[3]->change() == -1) {
      Serial.println("Someone crossed the IR sensor at pin 3");
      if (stateA) {
        count++;
        stateA = false;
        
        Serial.print("There are ");
        Serial.print(count);
        Serial.println(" in the room.");
      }
      else {
        stateB = true;
      }
    }*/
    
    handler->rateControl(); // Delay specified by the requirements from the client
  }
}
