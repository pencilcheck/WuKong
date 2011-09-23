#ifndef WUKONG_H
#define WUKONG_H

#include <stdlib.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "WProgram.h"

#define MAX_STRING_LENGTH 100
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define MAX_VIRTUAL_SENSORS 100
#else
#define MAX_VIRTUAL_SENSORS 50
#endif
//#define MAX_ID_NUMBER 50
#define CALLBACK_BACKLOG 10

#define STOP 0
#define START 1

#define READ 0x0
#define WRITE 0x1

#define DEBUG 1

#define BOARD_ID 12345

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
/*
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
*/
class VirtualSensor {
public:
  VirtualSensor(char* id, char* type, int pin, int interval, char* sensitivity, char* address, int mode)
  : _sensor_id(id), _type(type), _pin(pin), _interval(interval), _sensitivity(sensitivity), _address(address), _mode(mode), _val(0), _status(START) { pinMode(_pin, mode); };

  ~VirtualSensor() {
    delete _sensor_id; 
    delete _type;
    delete _sensitivity;
    delete _address;
  };

  int read() {
    if (_pin < A0) {
      _val = digitalRead(_pin);
      return _val;
    }
    else {
      _val = analogRead(_pin);
      return _val;
    }
  };

  void write(int val) {
    if (_pin < A0) {
      _val = val;
      digitalWrite(_pin, _val);
    }
    else {
      _val = val;
      analogWrite(_pin, _val);
    }
  };

  int read(int flag) {
    _val = digitalRead(_pin);
    return _val;
  };

  void write(int val, int flag) {
    _val = val;
    digitalWrite(_pin, _val);
  };

  void setSensitivity(char* sensitivity) { _sensitivity = sensitivity; };
  char* sensitivity() { return _sensitivity; };

  void setSensorId(char* sensor_id) { _sensor_id = sensor_id; };
  char* sensorId() { return _sensor_id; };

  void setAddress(char* address) { _address = address; };
  char* address() { return _address; };

  void setType(char* type) { _type = type; };
  int type() { return _type; };

  void setPin(int pin) { _pin = pin; };
  int pin() { return _pin; };

  void setInterval(int interval) { _interval = interval; };
  int interval() { return _interval; };

  void setMode(int mode) { _mode = mode; pinMode(_pin, _mode); };
  int mode() { return _mode; };

  int status() { return _status; };
  void start() { _status = START; };
  void stop() { _status = STOP; };

  struct pt proto() { return _proto; };

  void setMark(int mark) { _mark = mark; };
  int mark() { return _mark; };

  char* toString() {
    char response[MAX_STRING_LENGTH];
    sprintf(response, "%d:%d", _pin, _val);
    return response;
  };

protected:
  char* _sensor_id, * _type, * _sensitivity, * _address;
  int _mode, _status;
  int _pin, _val, _interval; // Do we need val?
  struct pt _proto;
  unsigned long _mark;
};

// Already defined in pins_arduino.h for each variant
// NUM_DIGITAL_PINS
// NUM_ANALOG_INPUTS
class Arduino {
public:
  Arduino()
  : _num_virtual_sensors(0) {};

  void addVirtualSensor(VirtualSensor* sensor) {
    _virtual_sensors[_num_virtual_sensors++] = sensor;
  };

  VirtualSensor* getVirtualSensor(char* id) {
    for (int i = 0; i < _num_virtual_sensors; ++i) {
      if (!strcmp(_virtual_sensors[i]->getId(), id)) {
        return _virtual_sensors[i];
      }
    }
    return NULL;
  };

  bool hasVirtualSensorId(char* id) {
    return getVirtualSensor(id) ? true : false;
  }

  void step() {
    char response[MAX_STRING_LENGTH];
    
    for (int i = 0; i < _num_virtual_sensors; ++i) {
      // run proto worker threads for tasks
      if (_virtual_sensors[i]->status() == START) {
        PT_INIT(&(_virtual_sensors[i]->proto()));
        if (_virtual_sensors[i]->mode() == READ) {
          if (i > 0)
            response = strcat(response, ",");
          pusher(&(_virtual_sensors[i]->proto()), _virtual_sensors[i], response);
        } else {
          pusher(&(_virtual_sensors[i]->proto()), _virtual_sensors[i], NULL);
        }
      }
    }

    if (strcmp(response, "")) {
      Serial.print("read ");
      Serial.println(response);
    }
  };

  void startVirtualSensor(char* id) {
    getVirtualSensor(id)->start();
  };

  void stopVirtualSensor(char* id) {
    getVirtualSensor(id)->stop();
  };

  // Hope for the best
  void deleteVirtualSensor(char* id) {
    delete getVirtualSensor(id);
  };

  //DigitalSensor** getDigitalSensors() { return _digital_sensors; };
  //int readDigitalSensor(int pin) { 
    //if (pin < _num_digital_pins)
      //return _digital_sensors[pin]->read(); 
    //else
      //return -1;
  //};

  //AnalogSensor** getAnalogSensors() { return _analog_sensors; };
  //int readAnalogSensor(int pin) { 
    //if (pin < _num_analog_pins)
      //return _analog_sensors[pin]->read(); 
    //else
      //return -1;
  //};
/*
  void readAll() {
    for (int i = 0; i < _num_analog_pins; i++)
      _analog_sensors[i]->read();
    for (int i = 0; i < _num_digital_pins; i++)
      _digital_sensors[i]->read();
  };
*/
protected:
  //int _num_digital_pins, _num_analog_pins;
  //DigitalSensor** _digital_sensors;
  //AnalogSensor** _analog_sensors;
  int _num_virtual_sensors;
  VirtualSensor* _virtual_sensors[MAX_VIRTUAL_SENSORS];
};

/*
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
*/

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
/*
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
*/

static int pusher(struct pt* pt, VirtualSensor* sensor, char* response) {
  PT_BEGIN(pt);

  while (1) {
    sensor->setMark(millis());
    PT_WAIT_UNTIL(pt, (millis() - sensor->mark()) % sensor->interval() == 0);
    if (sensor->mode() == WRITE) {
      sensor->write(HIGH);
    }
    else {
      sensor->read();
    }
    // only for read
    if (response) {
      strcat(response, (const char*)sensor->toString());
    }
  }

  PT_END(pt);
}


class RequestHandler {
public:
  RequestHandler(Arduino* board)
  : _board(board) {};

  void setBoard(Arduino* board) { _board = board; };
  Arduino* board() { return _board; };

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
    // INSERT
    // UPDATE
    // DELETE
    // READ
    // WRITE
    // DISABLE

    char* board_id = strtok(request, " ");
    if (board_id != BOARD_ID)
      return;
    char* command = strtok(NULL, " ");
    //sscanf(request, "%s %*s", command);

    char response[MAX_STRING_LENGTH];

    if (command == "read") {
      char* sensor;
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;
      while ((sensor = strtok(NULL, " ")) != NULL) {
        sensors[index++] = sensor;
      }

      sprintf(response, "success %s read sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {
        char* sensor_id;
        int interval;

        char* attribute = strtok(sensor, ",");
        char* key, * value;
        while (attribute != NULL) {
          sscanf(attribute, "%s:%s", key, value);
          if (!strcmp(key, "sensor_id")) {
            sensor_id = value;
          }
          else if (!strcmp(key, "interval")) {
            interval = atoi(value);
          }

          attribute = strtok(NULL, ",");
        }

        // Activate existing virtual sensor
        if (_board->hasVirtualSensorId(sensor_id)) {
          // Start sensor
          _board->getVirtualSensor(sensor_id)->setInterval(interval);
          _board->getVirtualSensor(sensor_id)->setMode(READ);
          _board->startVirtualSensor(sensor_id);

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensor_id);
        }
      }

      Serial.println(response);
    }
    else if (command == "write") {
      char* sensor;
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;
      while ((sensor = strtok(NULL, " ")) != NULL) {
        sensors[index++] = sensor;
      }

      sprintf(response, "success %s write sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {
        char* sensor_id;
        int sensor_value;

        char* attribute = strtok(sensor, ",");
        char* key, * value;
        while (attribute != NULL) {
          sscanf(attribute, "%s:%s", key, value);
          if (!strcmp(key, "sensor_id")) {
            sensor_id = value;
          }
          else if (!strcmp(key, "set_value")) {
            sensor_value = atoi(value);
          }

          attribute = strtok(NULL, ",");
        }

        // Activate existing virtual sensor
        if (_board->hasVirtualSensorId(sensor_id)) {
          // Start sensor
          _board->getVirtualSensor(sensor_id)->setMode(WRITE);
          _board->startVirtualSensor(sensor_id);

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensor_id);
        }
      }

      Serial.println(response);
    }
    else if (command == "disable") {
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;

      char* sensor = strtok(NULL, " ")
      sensor = strstr(sensor, ":")+1;
      char* id = strtok(sensor, ",");
      while (id != NULL) {
        sensors[index++] = id;
        id = strtok(NULL, ",");
      }

      sprintf(response, "success %s disable sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {
        // Activate existing virtual sensor
        if (_board->hasVirtualSensorId(sensors[j])) {
          // Stop sensor
          _board->stopVirtualSensor(sensors[j]);

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensor_id);
        }
      }

      Serial.println(response);
    }
    else if (command == "insert") {
      char* sensor;
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;
      while ((sensor = strtok(NULL, " ")) != NULL) {
        sensors[index++] = sensor;
      }

      sprintf(response, "success %s insert sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {
        char* sensor_id = genId();
        char* type = "";
        int pin = 0;
        int interval = 0;
        char* sensitivity = "";
        char* address = "";
        int mode = READ;

        char* attribute = strtok(sensor, ",");
        char* key, * value;
        while (attribute != NULL) {
          sscanf(attribute, "%s:%s", key, value);
          if (!strcmp(key, "type")) {
            type = value;
          }
          else if (!strcmp(key, "pin")) {
            pin = atoi(value);
          }
          else if (!strcmp(key, "interval")) {
            interval = atoi(value);
          }
          else if (!strcmp(key, "sensitivity")) {
            sensitivity = value;
          }
          else if (!strcmp(key, "address")) {
            address = value;
          }
          else if (!strcmp(key, "mode")) {
            mode = atoi(value);
          }

          attribute = strtok(NULL, ",");
        }

        // Activate existing virtual sensor
        if (_board->hasVirtualSensorId(sensor_id)) {
          // Create a virtual sensor
          // VirtualSensor(char* id, char* type, int pin, int interval, char* sensitivity, char* address, int mode)
          _board->addVirtualSensor(new VirtualSensor(sensor_id, type, pin, interval, sensitivity, address, mode));
          // Already started

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensor_id);
        }
      }

      Serial.println(response);
    }
    else if (command == "update") {
      // Implement later
    }
    else if (command == "delete") {
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;

      char* sensor = strtok(NULL, " ")
      sensor = strstr(sensor, ":")+1;
      char* id = strtok(sensor, ",");
      while (id != NULL) {
        sensors[index++] = id;
        id = strtok(NULL, ",");
      }

      sprintf(response, "success %s delete sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {
        // Activate existing virtual sensor
        if (_board->hasVirtualSensorId(sensors[j])) {
          // Stop sensor
          _board->deleteVirtualSensor(sensors[j]);

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensor_id);
        }
      }

      Serial.println(response);
    }
    else if (command == "describe") {
      // For later
    }
  };

  // Dummy call for simple security function
  bool authenticate(int id) {
    return false;
  };

  void error() {
    Serial.println("-1");
  };

private:
  Arduino* _board;
};

#endif
