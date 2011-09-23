#ifndef WUKONG_H
#define WUKONG_H

#include <stdlib.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "WProgram.h"

#define MAX_STRING_LENGTH 100
//#define MAX_ID_NUMBER 50
#define CALLBACK_BACKLOG 10
#define STOP 0
#define START 1
#define PAUSE 2

#define DEBUG 1

// Implementation of new for avr-gcc
void * operator new(size_t size) { return malloc(size); } 

// Implementation of delete for avr-gcc
void operator delete(void * ptr) { free(ptr); }

// For static keyword
__extension__ typedef int __guard __attribute__((mode (__DI__))); 

extern "C" int __cxa_guard_acquire(__guard *); 
extern "C" void __cxa_guard_release (__guard *); 
extern "C" void __cxa_guard_abort (__guard *); 

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);}; 
void __cxa_guard_release (__guard *g) {*(char *)g = 1;}; 
void __cxa_guard_abort (__guard *) {}; 

// For virtual
extern "C" void __cxa_pure_virtual(void); 
void __cxa_pure_virtual(void) {}; 


char* genId() {
  long randomNumber = random(MAX_ID_NUMBER);
  char id[3];
  sprintf(id, "%ld");
  return id;
};

template <typename T> class Command {
public:
  typedef void(T::*Action)();

  Command(T* object, Action method, unsigned long delay)
  : m_object(object), m_method(method), _delay(delay), _id(genId()), _status(1), _mark(0) {};

  void execute() {
    (m_object->*m_method)();
  };

  void setId(char* id) { _id = id; }

  char* id() { return _id; }

  void setDelay(unsigned long delay) { _delay = delay; }

  unsigned long delay() { return _delay; }

  void setMark(unsigned long mark) { _mark = mark; }

  unsigned long mark() { return _mark; }

  void setStatus(int status) { _status = status; }

  int status() { return _status; }

  struct pt proto() { return _proto; };

private:
  T* m_object;
  Action m_method;
  char _id[MAX_STRING_LENGTH];
  int _status; // 0 pause, 1 start
  unsigned long _delay;
  unsigned long _mark;
  struct pt _proto;
};

class BasicSensor {
public:
  BasicSensor(int pin)
  : _pin(pin), _val(0) {};

  virtual int read() {};

  virtual void write() {};

  void setPin(int pin) { _pin = pin; };
  int pin() { return _pin; };

  char* toString() {
    char response[MAX_STRING_LENGTH];
    sprintf(response, "%d:%d", _pin, _val);
    return response;
  };

protected:
  int _pin, _val;
};

class DigitalSensor: public BasicSensor {
public:
  DigitalSensor(int pin, int mode)
  : BasicSensor(pin) { _mode = mode; pinMode(_pin, mode); };

  int read() {
    _val = digitalRead(_pin);
    return _val;
  };

  void write(int val) {
    _val = val;
    digitalWrite(_pin, _val);
  };

  void setMode(int mode) { pinMode(_pin, mode); };
  int mode() { return _mode; };

private:
  int _mode;
};

class AnalogSensor: public BasicSensor {
public:
  AnalogSensor(int pin)
  : BasicSensor(pin) {};
      
  int read() {
    _val = analogRead(_pin);
    return _val;
  };

  void write(int val) {
    _val = val;
    analogWrite(_pin, _val);
  };
};

class Arduino {
public:
  Arduino(int dp, int ap)
  : _num_digital_pins(dp), _num_analog_pins(ap) {};

  DigitalSensor** getDigitalSensors() { return _digital_sensors; };
  int readDigitalSensor(int pin) { 
    if (pin < _num_digital_pins)
      return _digital_sensors[pin]->read(); 
    else
      return -1;
  };

  AnalogSensor** getAnalogSensors() { return _analog_sensors; };
  int readAnalogSensor(int pin) { 
    if (pin < _num_analog_pins)
      return _analog_sensors[pin]->read(); 
    else
      return -1;
  };
/*
  void readAll() {
    for (int i = 0; i < _num_analog_pins; i++)
      _analog_sensors[i]->read();
    for (int i = 0; i < _num_digital_pins; i++)
      _digital_sensors[i]->read();
  };
*/
protected:
  int _num_digital_pins, _num_analog_pins;
  DigitalSensor** _digital_sensors;
  AnalogSensor** _analog_sensors;
};

class ArduinoUno: public Arduino {
public:
  ArduinoUno()
  : Arduino(14, 6) {
    _digital_sensors = (DigitalSensor**)malloc(sizeof(DigitalSensor*)*_num_digital_pins);
    for (int i = 0; i < _num_digital_pins; ++i)
      _digital_sensors[i] = new DigitalSensor(i, INPUT);

    _analog_sensors = (AnalogSensor**)malloc(sizeof(AnalogSensor*)*_num_analog_pins);
    for (int i = 0; i < _num_analog_pins; ++i)
      _analog_sensors[i] = new AnalogSensor(i);
  };
};
/*
// Class specific to the board, should be developed by the developers
class InfraredUno: public ArduinoUno {
public:
  InfraredUno()
  : ArduinoUno(), _sep(2), _left_pin(2), _right_pin(3),
  _state(false), _state2(false) {
    // Assume the distance between the two IR sensors is known in cm
    // left/right is defined facing the sensor (IR facing you)
  };

  // Send 1 if the object is moving to the right of the sensor (IR facing you)
  // Send -1 otherwise (not working yet)
  // Send 0 if cannot determine
  void targetDirection() {
    // Identify patterns
    // Case: from pin to pin2
    // state0: sensors[pin] 0 -> 1, sensors[pin2] 0 -> 0
    // state1: sensors[pin2] 0 -> 1, sensors[pin] 1 -> 1
    // state2: sensors[pin] 1 -> 0, sensors[pin2] 1 -> 1
    // state3: sensors[pin2] 1 -> 0, sensors[pin] 0 -> 0

    DigitalSensor* left = getDigitalSensors()[_left_pin];
    DigitalSensor* right = getDigitalSensors()[_right_pin];

    if (!_state && !_state2) {
      if (left->change() == -1) {
        _state = true;
      }
    }
    else if (_state && !_state2) {
      if (right->change() == -1) {
        _state = false;
        Serial.print(1);
        return;
      }
    }
  };

  void targetLength() {
  };

  void targetSpeed() {
  };

  Command<InfraredUno>* taskFromRequest(char* request) {
    char* command = strtok(request, " ");
    if (command == "") {
      // Delay should be determined by WuKong but we are letting developers to
      // do that right now 5000 is 5 secs
      //Command<InfraredUno>* cmd = new Command<InfraredUno>(this, &InfraredUno::targetDirection, 5000);

      // Add one time driven callback as our subroutine 
      //_handler->addToPusher(cmd);
      //return cmd;
    }

    return NULL;
  };

private:
  int _sep;
  int _left_pin, _right_pin;
  int _state, _state2;
  int _speed;
};
#define BoardName InfraredUno
*/
/* End of custom board implementation */


//typedef Command<BoardName> Task;
//typedef Command<BasicSensor> InternalTask;


// Event driven handler
static int scheduler(struct pt* pt, Task* task) {
  PT_BEGIN(pt);

  while (1) {
    static unsigned long start = millis();
    // Execute callbacks, if returned false, terminate the thread and evict the
    // callback from the queue
    //if (!task->execute()) {
      //break;
    //}

    static unsigned long end = millis();
    // Will execute immediately if the execution time is longer than sample
    // rate
    PT_WAIT_UNTIL(pt, 2*start + task->delay() - end < millis()); // Delay specified by the requirements from the client
  }

  PT_END(pt);
}

// Not done
static int pusher(struct pt* pt, Task* task) {
  PT_BEGIN(pt);

  while (1) {
    task->setMark(millis());
    PT_WAIT_UNTIL(pt, (millis() - task->mark()) % task->delay() == 0);
    char response[MAX_STRING_LENGTH];
    //response = task->execute();
    Serial.print("push ");
    //Serial.println(response);
  }

  PT_END(pt);
}


class RequestHandler {
public:
  RequestHandler(Arduino* board)
  : _num_of_tasks(0), _board(board) {};

  void setBoard(Arduino* board) { _board = board; };

  Arduino* board() { return _board; };

  void addToQueue(Task* task) { _tasks[_num_of_tasks++] = task; }

  char* receiveRequest() {
    char request[MAX_STRING_LENGTH];
    int index = 0;

    while (Serial.available()) {
      request[index++] = Serial.read();
      delay(1); // Need to experiment with it
    }
    request[index] = '\0';

    return request;
  };

  void handle(char request[]) {
    // DESCRIBE
    // (UN)READ
    // (UN)WRITE
    // CONFIG
    // INSERT
    // DELETE
    // UPDATE

    char* command = strtok(request, " ");
    //sscanf(request, "%s %*s", command);

    if (command == "push") {
      char response[MAX_STRING_LENGTH];
      int index = 0; index += sprintf(response, "push ");

      char* type = strtok(NULL, " ");
      char* pinAndIntervals = strtok(NULL, " ");
      while (type && pinAndIntervals) {
        if (type == "D") {
          char* pinAndInterval = strtok(pinAndIntervals, ",");
          char* pin;
          unsigned long interval;
          sscanf(pinAndInterval, "%s:%ld", pin, &interval);

          while (pinAndInterval != NULL) {
            //addToPusher(new InternalTask(_board->getDigitalSensors()[atoi(pin)], BasicSensor::toString, interval));

            int value = _board->readDigitalSensor(atoi(pin));
            if (value != -1) {
              index += sprintf(response + index, "%s:%d", pin, value);
            }
            pinAndInterval = strtok(NULL, ",");
            if (pin != NULL)
              index += sprintf(response + index, ",");
          }
        }
        else if (type == "A") {
          char* pinAndInterval = strtok(pinAndIntervals, ",");
          char* pin;
          unsigned long interval;
          sscanf(pinAndInterval, "%s:%ld", pin, &interval);

          while (pinAndInterval != NULL) {
            //addToPusher(new InternalTask(_board->getAnalogSensors()[atoi(pin)], BasicSensor::toString, interval));

            int value = _board->readAnalogSensor(atoi(pin));
            if (value != -1) {
              index += sprintf(response + index, "%s:%d", pin, value);
            }
            pinAndInterval = strtok(NULL, ",");
            if (pin != NULL)
              index += sprintf(response + index, ",");
          }
        }

        type = strtok(NULL, " ");
        pinAndIntervals = strtok(NULL, " ");
      }
      // Response below
      Serial.println(response);
    }
    else if (command == "pull") {
      char response[MAX_STRING_LENGTH];
      int index = 0; index += sprintf(response, "pull ");

      char* type = strtok(NULL, " ");
      char* pins = strtok(NULL, " ");
      while (type && pins) {
        if (type == "D") {
          char* pin = strtok(pins, ",");
          while (pin != NULL) {
            int value = _board->readDigitalSensor(atoi(pin));
            if (value != -1) {
              index += sprintf(response + index, "%s:%d", pin, value);
            }
            pin = strtok(NULL, ",");
            if (pin != NULL)
              index += sprintf(response + index, ",");
          }
        }
        else if (type == "A") {
          char* pin = strtok(pins, ",");
          while (pin != NULL) {
            int value = _board->readAnalogSensor(atoi(pin));
            if (value != -1) {
              index += sprintf(response + index, "%s:%d", pin, value);
            }
            pin = strtok(NULL, ",");
            if (pin != NULL)
              index += sprintf(response + index, ",");
          }
        }

        type = strtok(NULL, " ");
        pins = strtok(NULL, " ");
      }
      // Response below
      Serial.println(response);
    }
    else if (command == "stop") {
      char* type = strtok(NULL, " ");
      char* pins = strtok(NULL, " ");
      while (type && pins) {

      }
      // Response below
    }
    else if (command == "write") {
      char* type = strtok(NULL, " ");
      char* pinAndValues = strtok(NULL, " ");
      while (type && pinAndValues) {

      }
      // Response below
    }
    else if (command == "config") {
      char* keyAndValues = strtok(NULL, " ");
      while (keyAndValues) {

      }
      // Response below
    }
  };

  void step() {
    for (int i = 0; i < _num_of_tasks; ++i) {
      // run proto worker threads for tasks
      if (_tasks[i]->status() == START) {
        PT_INIT(&(_internal_tasks[i]->proto()));
        pusher(&(_internal_tasks[i]->proto()), _internal_tasks[i]);
      }
    }
  }

  // Dummy call for simple security function
  bool authenticate(int id) {
    return false;
  };

  void error() {
    Serial.println("-1");
  };

private:
  Arduino* _board;
  int _num_of_tasks;
  Task* _tasks[CALLBACK_BACKLOG];
  int _num_of_internal_tasks;
  InternalTask* _internal_tasks[CALLBACK_BACKLOG];
};




#endif
