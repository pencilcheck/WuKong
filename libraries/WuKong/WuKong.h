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
#define MAX_ID_NUMBER 50
#define CALLBACK_BACKLOG 10

#define STOP 0
#define START 1

#define READ 0x0
#define WRITE 0x1

#define DEBUG(msg) Serial.print("DEBUG:");Serial.println(msg);

#define BOARD_ID "12345"

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

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
int pinMapping(char* pin) {
  if (pin[0] == 'A') {
    return atoi((const char*)pin+1) + A0;
  }
  else {
    return atoi((const char*)pin);
  }
}
#else
int pinMapping(char* pin) {
  if (pin[0] == 'A') {
    return atoi((const char*)pin+1) + A0;
  }
  else {
    return atoi((const char*)pin);
  }
}
#endif


static int maxId = 0;

char* genId() {
  //long randomNumber = random(MAX_ID_NUMBER);
  char* id = (char*)malloc(3*sizeof(char));
  sprintf(id, "%d", maxId++);

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
  : _sensor_id(id), _type(type), _pin(pin), _interval(interval), _sensitivity(sensitivity), _address(address), _mode(mode), _val(0), _status(STOP), _hasRead(false), _set_value(0) 
  { 
    pinMode(_pin, mode); 
    PT_INIT(&_proto); 
  };

  ~VirtualSensor() {
    free(_sensor_id); 
    free(_type);
    free(_sensitivity);
    free(_address);
  };

  int hasRead() {
    return _hasRead;
  };
  void setHasRead(bool read) {
    _hasRead = read;
  };

  int read() {
    if (_pin < A0) {
      _val = digitalRead(_pin);
      return _val;
    }
    else {
      int pin = _pin - A0;
      _val = analogRead(pin);
      return _val;
    }
  };

  void write(int val) {
    if (_pin < A0) {
      _val = val;
      digitalWrite(_pin, _val);
    }
    else {
      int pin = _pin - A0;
      _val = val;
      analogWrite(pin, _val);
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

  void setSetValue(int set_value) { _set_value = set_value; };
  int setValue() { return _set_value; };

  void setSensitivity(char* sensitivity) { _sensitivity = sensitivity; };
  char* sensitivity() { return _sensitivity; };

  void setSensorId(char* sensor_id) { _sensor_id = sensor_id; };
  char* sensorId() { return _sensor_id; };

  void setAddress(char* address) { _address = address; };
  char* address() { return _address; };

  void setType(char* type) { _type = type; };
  char* type() { return _type; };

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
    char response[MAX_STRING_LENGTH]; memset(response, 0, MAX_STRING_LENGTH);
    //sprintf(response, "%d:%d", _pin, _val);

    char pin[MAX_STRING_LENGTH]; memset(pin, 0, MAX_STRING_LENGTH);
    char val[MAX_STRING_LENGTH]; memset(val, 0, MAX_STRING_LENGTH);
    itoa(_pin, pin, 10);
    itoa(_val, val, 10);

    strcat(response, pin);
    strcat(response, ":");
    strcat(response, val);

    return response;
  };

protected:
  char* _sensor_id, * _type, * _sensitivity, * _address;
  int _mode, _status;
  int _pin, _val, _interval; // Do we need val?
  int _set_value;
  struct pt _proto;
  unsigned long _mark;
  bool _hasRead;
};

// Already defined in pins_arduino.h for each variant
// NUM_DIGITAL_PINS
// NUM_ANALOG_INPUTS
class Arduino {
public:
  Arduino() {
    for (int i = 0; i < MAX_VIRTUAL_SENSORS; ++i) {
      _virtual_sensors[i] = NULL;
    }
  };

  void addVirtualSensor(VirtualSensor* sensor) {
    for (int i = 0; i < MAX_VIRTUAL_SENSORS; ++i) {
      if (_virtual_sensors[i] == NULL) {
        _virtual_sensors[i] = sensor;
        break;
      }
    }
  };

  VirtualSensor* getVirtualSensor(char* id) {
    for (int i = 0; i < MAX_VIRTUAL_SENSORS; ++i) {
      if (_virtual_sensors[i] != NULL) {
        if (!strcmp(_virtual_sensors[i]->sensorId(), id)) {
          return _virtual_sensors[i];
        }
      }
    }
    return NULL;
  };

  bool hasVirtualSensor(char* id) {
    return getVirtualSensor(id) ? true : false;
  }

  void step() {
    char response[MAX_STRING_LENGTH];
    memset(response, 0, MAX_STRING_LENGTH);
    
    int count = 0;
    for (int i = 0; i < MAX_VIRTUAL_SENSORS; ++i) {
      if (_virtual_sensors[i] != NULL) {
        // run proto worker threads for tasks
        if (_virtual_sensors[i]->status() == START) {
          //PT_INIT(&(_virtual_sensors[i]->proto()));
          if (_virtual_sensors[i]->mode() == READ) {
            if (count > 0)
              strcat(response, ",");

            //pusher(&(_virtual_sensors[i]->proto()), _virtual_sensors[i], response);
            if(pusher(_virtual_sensors[i], response))
              count++;
          }
        }
      }
    }

    if (strlen(response) > 0) {
      Serial.print("read ");
      Serial.println(response);
    }
  };

  void startVirtualSensor(char* id) {
    if (hasVirtualSensor(id)) {
      getVirtualSensor(id)->start();
    }
  };

  void stopVirtualSensor(char* id) {
    if (hasVirtualSensor(id)) {
      getVirtualSensor(id)->stop();
    }
  };

  // Hope for the best
  bool deleteVirtualSensor(char* id) {
    DEBUG("deleteVirtualSensor");

    if (hasVirtualSensor(id)) {
      for (int i = 0; i < MAX_VIRTUAL_SENSORS; ++i) {
        if (_virtual_sensors[i] != NULL) {
          if (!strcmp(_virtual_sensors[i]->sensorId(), id)) {
            free(_virtual_sensors[i]);
            _virtual_sensors[i] = NULL;
            break;
          }
        }
      }

      DEBUG("delete success");

      return true;
    }
    return false;
  };

  bool pusher(VirtualSensor* sensor, char* response) {

    if (!sensor->hasRead()) {
      sensor->setMark(millis());
      sensor->setHasRead(true);
    }

    if((millis() - sensor->mark()) % sensor->interval() == 0) {

      sensor->read();

      strcat(response, (const char*)sensor->toString());

      sensor->setMark(millis());
      return true;
    }
    return false;
  }
/*
  static PT_THREAD(pusher(struct pt* pt, VirtualSensor* sensor, char* response) ){
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
      if (response != NULL) {
        strcat(response, (const char*)sensor->toString());
      }
    }

    PT_END(pt);
  }
*/
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
  //int _num_virtual_sensors;
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
      delay(10); // Need to experiment with it
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

    if (strcmp(board_id, BOARD_ID))
      return;

    char* command = strtok(NULL, " ");
    //sscanf(request, "%s %*s", command);

    char response[MAX_STRING_LENGTH];

    if (!strcmp(command, "read")) {
      char* sensor;
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;
      while ((sensor = strtok(NULL, " ")) != NULL) {
        sensors[index++] = sensor;
      }

      sprintf(response, "success %s read sensor_id:", BOARD_ID);
      int intervalIndex = 0;

      char noIntervalResponse[MAX_STRING_LENGTH]; memset(noIntervalResponse, 0, MAX_STRING_LENGTH);
      int noIntervalIndex = 0;

      for (int j = 0; j < index; ++j) {
        char sensor_id[MAX_STRING_LENGTH]; memset(sensor_id, 0, MAX_STRING_LENGTH);
        int interval = -1;

        char* attribute = strtok(sensors[j], ",");
        while (attribute) {

          char key[MAX_STRING_LENGTH], value[MAX_STRING_LENGTH];
          memset(key, 0, MAX_STRING_LENGTH);
          memset(value, 0, MAX_STRING_LENGTH);
          sscanf(attribute, "%[^:]:%s", key, value);


          if (!strcmp(key, "sensor_id")) {
            strcpy(sensor_id, value);
          }
          else if (!strcmp(key, "interval")) {
            interval = atoi(value);
          }

          attribute = strtok(NULL, ",");
        }


        // Activate existing virtual sensor
        if (_board->hasVirtualSensor(sensor_id)) {

          if (interval != -1) {
            // Start sensor
            _board->getVirtualSensor(sensor_id)->setInterval(interval);
            _board->getVirtualSensor(sensor_id)->setMode(READ);
            _board->startVirtualSensor(sensor_id);
          }
          else {
            _board->getVirtualSensor(sensor_id)->read();
            if (noIntervalIndex > 0)
              strcat(noIntervalResponse, ",");
            strcat(noIntervalResponse, (const char*)_board->getVirtualSensor(sensor_id)->toString());
            noIntervalIndex++;
          }

          // Concatenate to response
          if (intervalIndex > 0)
            strcat(response, ",");
          strcat(response, sensor_id);
          intervalIndex++;
        }
      }

      if (intervalIndex > 0) {
        Serial.println(response);
      }
      else {
        // Fail message
        Serial.println("failure read");
      }

      if (noIntervalIndex > 0) {
        Serial.print("read ");
        Serial.println(noIntervalResponse);
      }
    }
    else if (!strcmp(command, "write")) {
      char* sensor;
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;
      while ((sensor = strtok(NULL, " ")) != NULL) {
        sensors[index++] = sensor;
      }

      sprintf(response, "success %s write sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {

        char sensor_id[MAX_STRING_LENGTH]; memset(sensor_id, 0, MAX_STRING_LENGTH);
        int set_value = 0;

        char* attribute = strtok(sensors[j], ",");
        while (attribute) {

          char key[MAX_STRING_LENGTH], value[MAX_STRING_LENGTH];
          memset(key, 0, MAX_STRING_LENGTH);
          memset(value, 0, MAX_STRING_LENGTH);
          sscanf(attribute, "%[^:]:%s", key, value);


          if (!strcmp(key, "sensor_id")) {
            strcpy(sensor_id, value);
          }
          else if (!strcmp(key, "set_value")) {
            set_value = atoi(value);
          }

          attribute = strtok(NULL, ",");
        }


        // Activate existing virtual sensor
        if (_board->hasVirtualSensor(sensor_id)) {

          // Start sensor
          _board->getVirtualSensor(sensor_id)->setSetValue(set_value);
          _board->getVirtualSensor(sensor_id)->setMode(WRITE);


          // TODO: for later, analogWrite are only available through PWM on
          // certain "digital" pins, needs to be aware of that
          _board->getVirtualSensor(sensor_id)->write(_board->getVirtualSensor(sensor_id)->setValue());

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensor_id);
        }
      }

      Serial.println(response);
    }
    else if (!strcmp(command, "disable")) {
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;

      char* sensor = strtok(NULL, " ");
      sensor = strstr(sensor, ":")+1;
      char* id = strtok(sensor, ",");
      while (id != NULL) {
        sensors[index++] = id;
        id = strtok(NULL, ",");
      }

      sprintf(response, "success %s disable sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {
        // Activate existing virtual sensor
        if (_board->hasVirtualSensor(sensors[j])) {
          // Stop sensor
          _board->stopVirtualSensor(sensors[j]);

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensors[j]);
        }
      }

      Serial.println(response);
    }
    else if (!strcmp(command, "insert")) {

      char* sensor;
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;
      while ((sensor = strtok(NULL, " ")) != NULL) {
        sensors[index++] = sensor;
      }

      sprintf(response, "success %s insert sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {

        char* sensor_id = genId();

        char type[MAX_STRING_LENGTH]; memset(type, 0, MAX_STRING_LENGTH);
        int pin = 0;
        int interval = 0;
        char sensitivity[MAX_STRING_LENGTH]; memset(type, 0, MAX_STRING_LENGTH);
        char address[MAX_STRING_LENGTH]; memset(address, 0, MAX_STRING_LENGTH);
        int mode = READ;

        char* attribute = strtok(sensors[j], ",");
        while (attribute) {

          char key[MAX_STRING_LENGTH], value[MAX_STRING_LENGTH];
          memset(key, 0, MAX_STRING_LENGTH);
          memset(value, 0, MAX_STRING_LENGTH);
          sscanf(attribute, "%[^:]:%s", key, value);


          if (!strcmp(key, "type")) {
            strcpy(type, value);
          }
          else if (!strcmp(key, "pin")) {
            pin = pinMapping(value);
          }
          else if (!strcmp(key, "interval")) {
            interval = atoi(value);
          }
          else if (!strcmp(key, "sensitivity")) {
            strcpy(sensitivity, value);
          }
          else if (!strcmp(key, "address")) {
            strcpy(address, value);
          }
          else if (!strcmp(key, "mode")) {
            mode = atoi(value);
          }

          attribute = strtok(NULL, ",");
        }

        // Insert new virtual sensor, if id exist then don't create
        if (!_board->hasVirtualSensor(sensor_id)) {

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

      // Fail response if there is no added new sensors
      Serial.println(response);
    }
    else if (!strcmp(command, "update")) {
      // Implement later
    }
    else if (!strcmp(command, "delete")) {
      char* sensors[MAX_VIRTUAL_SENSORS];
      int index = 0;

      char* sensor = strtok(NULL, " ");
      sensor = strstr(sensor, ":")+1;
      char* id = strtok(sensor, ",");
      while (id != NULL) {
        sensors[index++] = id;
        id = strtok(NULL, ",");
      }

      sprintf(response, "success %s delete sensor_id:", BOARD_ID);

      for (int j = 0; j < index; ++j) {
        // Activate existing virtual sensor
        if (_board->hasVirtualSensor(sensors[j])) {
          // Stop sensor
          _board->stopVirtualSensor(sensors[j]);
          if (_board->deleteVirtualSensor(sensors[j])) {
            // TODO:Success
          }
          else {
            // TODO:Fail
          }

          // Concatenate to response
          if (j > 0)
            strcat(response, ",");
          strcat(response, sensors[j]);
        }
      }

      Serial.println(response);
    }
    else if (!strcmp(command, "describe")) {
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
