#ifndef WUKONG_H
#define WUKONG_H

#include <stdlib.h> 
#include "WProgram.h"

#define MAX_REQ_LENGTH 5000
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

    virtual bool hasFeature(char* request) {};

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

class InfraredUno: public ArduinoUno {
  public:
    InfraredUno()
    : ArduinoUno() {
      // Assume we know the distance between the two direction IR sensors in cm
      _sep = 5;
      _left_pin = 2;
      _right_pin = 3; // facing the sensor (IR facing you)
    };

    // Return 1 if the object is moving to the right of the sensor (IR facing you)
    // return -1 otherwise
    // return 0 if cannot determine
    int targetDirection() {
      // Identify patterns
      // sensors[pin] 0 -> 1, sensors[pin2] 0 -> 0
      // sensors[pin2] 0 -> 1, sensors[pin] 1 -> 1
      // sensors[pin] 1 -> 0, sensors[pin2] 1 -> 1
      // sensors[pin2] 1 -> 0, sensors[pin] 0 -> 0
    };

    int targetLength() {
    };

    int targetSpeed() {
    };

  private:
    int _sep;
    int _left_pin, _right_pin;
    int _speed;
};

class RequestHandler {
  public:
    RequestHandler()
    : _delay_in_millis(0), _board(new InfraredUno()) {
      // TODO:A way to self-discovery to know which board this is
    };

    void handle(char* request) {
      // Request format: [COMMAND]: [VALUE]: [APP_ID]
      // Return format: [APP_ID]: [VALUE]
      // JOIN: 0: 0 -> 15: 0
      // LEAVE: 0: 15 -> 15: 0
      // DESCRIBE: 0: 15 -> 15: [features, ..]
      char command[20];
      char value[20];
      sscanf(request, "%s: %s: %s", command, value);


      // Since we should know about the board features ahead of time, we can
      // explicitly tell the client which features we support
      // e.g. self-reconfiguration, inspect, some policies, some requirements
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
};

#endif
