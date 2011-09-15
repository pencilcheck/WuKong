#ifndef WUKONG_H
#define WUKONG_H

#include <stdlib.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

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


template <typename T> class Command {
public:
  typedef void(T::*Action)();

  Command(T* object, Action method, unsigned long delay) {
    m_object = object;
    m_method = method;
    _flag = false;
    _delay = delay;
  };

  void start() {
    _flag = false;
  }

  void stop() {
    _flag = true;
  }

  bool started() {
    return !_flag;
  }

  bool execute() {
    if (!_flag) {
      (m_object->*m_method)();
      return true;
    }
    return false;
  };

  void setId(int id) {
    _id = id;
  }

  int id() {
    return _id;
  }

  unsigned long delay() {
    return _delay;
  }

  bool terminated() {
    return _flag;
  }

private:
  T* m_object;
  Action m_method;
  int _id;
  bool _flag; // Termination flag
  unsigned long _delay;
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
  };

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

  Command<InfraredUno>* handleRequest(char request) {
    if (request == 'C') {
      // Delay should be determined by WuKong but we are letting developers to
      // do that right now 5000 is 5 secs
      Command<InfraredUno>* cmd = new Command<InfraredUno>(this, &InfraredUno::targetDirection, 5000);

      // Add one time driven callback as our subroutine 
      //_handler->addToScheduler(cmd);
      return cmd;
    }
    else
      return NULL;
  };

private:
  int _sep;
  int _left_pin, _right_pin;
  int _state, _state2;
  int _speed;
};

#define BoardName InfraredUno

/* End of custom board implementation */


typedef Command<BoardName> Task;

// Time driven scheduler
static int scheduler(struct pt* pt, Task* task) {
  PT_BEGIN(pt);

  while (1) {
    static unsigned long start = millis();
    // Execute callbacks, if returned false, terminate the thread and evict the
    // callback from the queue
    if (!task->execute()) {
      break;
    }

    static unsigned long end = millis();
    // Will execute immediately if the execution time is longer than sample
    // rate
    PT_WAIT_UNTIL(pt, 2*start + task->delay() - end < millis()); // Delay specified by the requirements from the client
  }

  PT_END(pt);
}


class RequestHandler {
public:
  RequestHandler(Arduino* board)
  : _num_of_tasks(0), _num_of_clients(0), _board(board) {};

  char receiveRequest() {
    char request;

    if (Serial.available()) {
      char inByte = Serial.read();
      request = inByte;
    }

    return request;
  };

  void handle(char request) {
    // Request format: [COMMAND]
    // Return format: [VALUE]
    // JOIN
    // LEAVE
    // DESCRIBE
    // COUNT

    // Since we should know about the board features ahead of time, we can
    // explicitly tell the client which features we support
    // e.g. self-reconfiguration, inspect, some policies, some requirements

    int value;
    int id;
    //sscanf(request, "%s %*s", command);

    if (request == 'J') {
      id = random(MAX_ID_NUMBER);
      _ids[_num_of_clients++] = id;
      Serial.print(id);
    }
    else if (request == 'D') {
    }
    else {
      //sscanf(request, "%*s %d", &id);
      addToScheduler(((BoardName*)_board)->handleRequest(request));

      // Start scheduler and interrupt handler if there is any
      startAll();
    }
  };

  void addToScheduler(Task* task) {
    task->setId(_num_of_tasks);
    _tasks[_num_of_tasks++] = task;
  }

  void startAll() {
    for (int i = 0; i < _num_of_tasks; ++i) {
      // Spawn worker threads
      if (!_tasks[i]->started()) {

        _tasks[i]->start();
        int id = _tasks[i]->id();
        // Start the thread
        struct pt proto;
        PT_INIT(&proto);
        scheduler(&proto, _tasks[i]);
      }
    }

    // Hook interrupts to events
  }

  // Execute task with specific id
  bool execute(int id) {
    for (int i = 0; i < _num_of_tasks; i++) {
      if (_tasks[i]->id() == id) {
        return _tasks[i]->execute();
      }
    }

    return false;
  };

  Arduino* board() {
    return _board;
  };

  // Dummy call for simple security function
  bool authenticate(int id) {
    for (int i = 0; i < _num_of_clients; ++i) {
      if (_ids[i] == id)
        return true;
    }
    return false;
  };

  void error() {
    Serial.println("-1");
  };

private:
  Arduino* _board;
  int _num_of_clients;
  int _ids[MAX_ID_NUMBER];
  int _num_of_tasks;
  Task* _tasks[CALLBACK_BACKLOG];
};



#endif
