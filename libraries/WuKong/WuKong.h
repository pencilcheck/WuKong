#ifndef WUKONG_H
#define WUKONG_H

#include <stdlib.h> 
#include "WProgram.h"

#define MAX_ID_NUMBER 50
#define MAX_REQ_LENGTH 5000
#define CALLBACK_BACKLOG 10
#define DEBUG 1

// Implementation of new for avr-gcc
void * operator new(size_t size) 
{ 
  return malloc(size); 
} 

// Implementation of delete for avr-gcc
void operator delete(void * ptr) 
{ 
  free(ptr); 
}

template <typename T> class Command {
  public:
    typedef void(T::*Action)();
    Command(T* object, Action method) {
      m_object = object;
      m_method = method;
    };

    void execute() {
      (m_object->*m_method)();
    };
  private:
    T* m_object;
    Action m_method;
};

class BasicSensor {
  public:
    BasicSensor(int pin, int mode)
    : _pin(pin), _val(0), _change(0) {
      pinMode(_pin, mode);
    };

    virtual int read() {};

    virtual void write() {};

    void setPin(int pin) {
      _pin = pin;
    };

    void setMode(int mode) {
      pinMode(_pin, mode);
    };

    int val() {
      return _val;
    };

    int change() {
      return _change;
    };

  protected:
    int _pin;
    int _change;
    int _val;
};

class DigitalSensor: public BasicSensor {
  public:
    DigitalSensor(int pin, int mode)
    : BasicSensor(pin, mode) {};
        
    int read() {
      int val = digitalRead(_pin);

      if (val > _val) _change = 1;
      else if (val < _val) _change = -1;
      else _change = 0;

      _val = val;
      return _val;
    };

    void write(int value) {
      digitalWrite(_pin, value);

      if (value > _val) _change = 1;
      else if (value < _val) _change = -1;
      else _change = 0;

      _val = value;
    };
};

class AnalogSensor: public BasicSensor {
  public:
    AnalogSensor(int pin, int mode)
    : BasicSensor(pin, mode) {};
        
    int read() {
      int val = analogRead(_pin);

      if (val > _val) _change = 1;
      else if (val < _val) _change = -1;
      else _change = 0;

      _val = val;
      return _val;
    };

    void write(int value) {
      analogWrite(_pin, value);

      if (value > _val) _change = 1;
      else if (value < _val) _change = -1;
      else _change = 0;

      _val = value;
    };
};


class Arduino {
  public:
    Arduino()
    : _num_digital_pins(0), _num_analog_pins(0) {};

    DigitalSensor** getDigitalSensors() {
      return _digital_sensors;
    };

    AnalogSensor** getAnalogSensors() {
      return _analog_sensors;
    };

    void readAll() {
      for (int i = 0; i < _num_analog_pins; i++)
        _analog_sensors[i]->read();
      for (int i = 0; i < _num_digital_pins; i++)
        _digital_sensors[i]->read();
    }

  protected:
    int _num_digital_pins;
    int _num_analog_pins;
    DigitalSensor** _digital_sensors;
    AnalogSensor** _analog_sensors;
};

class ArduinoUno: public Arduino {
  public:
    ArduinoUno()
    : Arduino() {
      _num_digital_pins = 14;
      _num_analog_pins = 6;
      _digital_sensors = (DigitalSensor**)malloc(sizeof(DigitalSensor*)*_num_digital_pins);
      for (int i = 0; i < _num_digital_pins; ++i) {
        _digital_sensors[i] = new DigitalSensor(i, INPUT);
      }

      _analog_sensors = (AnalogSensor**)malloc(sizeof(AnalogSensor*)*_num_analog_pins);
      for (int i = 0; i < _num_analog_pins; ++i) {
        _analog_sensors[i] = new AnalogSensor(i, INPUT);
      }
    };
};

// Class specific to the board, should be developed by the developers
class InfraredUno: public ArduinoUno {
  public:
    InfraredUno()
    : ArduinoUno(), _sep(2), _left_pin(2), _right_pin(3) {
      // Assume the distance between the two IR sensors is known in cm
      // left/right is defined facing the sensor (IR facing you)
    };

    // Send 1 if the object is moving to the right of the sensor (IR facing you)
    // Send -1 otherwise
    // Send 0 if cannot determine
    void targetDirection() {
      Serial.println("entering targetDirection");
      // Identify patterns
      // Case: from pin to pin2
      // state0: sensors[pin] 0 -> 1, sensors[pin2] 0 -> 0
      // state1: sensors[pin2] 0 -> 1, sensors[pin] 1 -> 1
      // state2: sensors[pin] 1 -> 0, sensors[pin2] 1 -> 1
      // state3: sensors[pin2] 1 -> 0, sensors[pin] 0 -> 0

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
    };

    void targetLength() {
    };

    void targetSpeed() {
    };

    Command<InfraredUno>* newRequest(char* method) {
      if (method == "COUNT")
        return new Command<InfraredUno>(new InfraredUno, &InfraredUno::targetDirection);
      else
        return NULL;
    };

  private:
    int _sep;
    int _left_pin, _right_pin;
    int _speed;
};

typedef Command<InfraredUno> Request;
// to here

class RequestHandler {
  public:
    RequestHandler(Arduino* board)
    : _delay_in_millis(0), _num_of_clients(0), _board(board), _num_callbacks(0) {};

    bool checkForId(int id) {
      for (int i = 0; i < _num_of_clients; ++i) {
        if (_ids[i] == id)
          return true;
      }
      return false;
    };

    void error() {
      Serial.println("-1");
    };

    void handle(char* request) {
      // Request format: [COMMAND] [APP_ID]
      // Return format: [VALUE] [APP_ID]
      // JOIN: 0: 0 -> 15: 0
      // LEAVE: 0: 15 -> 15: 0
      // DESCRIBE: 0: 15 -> 15: [features, ..]

      // Since we should know about the board features ahead of time, we can
      // explicitly tell the client which features we support
      // e.g. self-reconfiguration, inspect, some policies, some requirements

      char command[20];
      int value;
      int id;
      sscanf(request, "%s %*s", command);

      if (command == "JOIN") {
        Serial.println(id);
      }
      else if (command == "DESCRIBE") {
        sscanf(request, "%*s %d", &id);
        if (checkForId(id))
          Serial.println();
      }
      else if (command == "COUNT") {
        sscanf(request, "%*s %d", &id);
        if (checkForId(id)) {
          //Request* cmd = new Request(new InfraredUno, &InfraredUno::targetDirection);
          Request* cmd = ((InfraredUno*)_board)->newRequest(command);
          appendCallback(cmd);
        }
      }
      else if (command == "SAMPLERATE") {
        sscanf(request, "%*s %d %d", &id, &value);
        _delay_in_millis = value;
      }
    };

    void oneStep() {
      // Call callbacks one by one
      for (int i = 0; i < _num_callbacks; i++) {
        _callbacks[i]->execute();
      }
    };

    void appendCallback(Request* cmd) {
      // Add one callback as our subroutine
      _callbacks[_num_callbacks++] = cmd;
    };

    void readAll() {
      _board->readAll();
    };

    void rateControl() {
      delay(_delay_in_millis);
    };

    char* receiveRequest() {
      char request[MAX_REQ_LENGTH];
      int index = 0;
      while (Serial.available()) {
        request[index++] = Serial.read();
      }
      request[index] = '\0';

      return request;
    };

    Arduino* board() {
      return _board;
    };

  private:
    Arduino* _board;
    unsigned long _delay_in_millis;
    int _num_of_clients;
    int _ids[MAX_ID_NUMBER];
    int _num_callbacks;
    Request* _callbacks[CALLBACK_BACKLOG];
};

#endif
