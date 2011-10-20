import serial
import os, sys

port = '/dev/ttyACM' + sys.argv[1]
print "Connecting to", port
sp = serial.Serial(port, 9600)

ON = 0
OFF = 1

state1 = 0
state2 = 0
#count = 0
doorState = False # False closed, True opened

# because i'm in the room when I run this program so count has to be 1
count = 1

left = 0
right = 0
prevDoor = 1
door = 1

if os.path.exists('count.txt'):
  os.remove('count.txt')

f = open('count.txt', 'w')
f.write(str(count))
f.close()

d = open('door.txt', 'w')
d.write(str(door))
d.close()

class DoorClass:
  def __init__(self):
    self.opened = False
    self.state = 0

  def getState(self, value):
    if (self.state == 0):
      if (value == ON):
        self.state = 1
    elif (self.state == 1):
      if (value == OFF):
        self.state = 0
        self.opened = not self.opened
        if self.opened:
          print "door opened"
        else:
          print "door closed"


    return self.opened


doorClass = DoorClass()
dirty = True


while True:
  line = sp.readline()
  outside = int(line[0])
  inside = int(line[1])
  door = int(line[2])
  #print outside, inside, door


  if not doorClass.getState(door):
    state1 = 0
    state2 = 0
    dirty = True
    if os.path.exists('door.txt'):
      os.remove('door.txt')

    d = open('door.txt', 'w')
    d.write('CLOSE')
    d.close()
  else:
    if os.path.exists('door.txt'):
      os.remove('door.txt')

    d = open('door.txt', 'w')
    d.write('OPEN')
    d.close()

    #if doorState and door == OFF and prevDoor == OFF: # after the light is turned off
    if inside == ON and dirty:
      pass
    else:
      print line
      dirty = False
      if state1 == 0 and state2 == 0:
        if outside == ON and inside == OFF:
          state1 = 1
          #print "state1: coming in"
        elif outside == OFF and inside == ON:
          state2 = 1
          #print "state2: going out"

      if state1 != 0:
        if state1 == 1 and outside == ON and inside == ON:
          state1 = 2
        elif state1 == 1 and outside == OFF and inside == ON:
          state1 = 3
        elif state1 == 2 and outside == OFF and inside == ON:
          state1 = 3
        elif (state1 == 2 or state1 == 3) and outside == OFF and inside == OFF:
          count += 1
          print count, "people here"
          if os.path.exists('count.txt'):
            os.remove('count.txt')

          f = open('count.txt', 'w')
          f.write(str(count))
          f.close()
          state1 = 0
      elif state2 != 0:
        if state2 == 1 and outside == ON and inside == ON:
          state2 = 2
        elif state2 == 1 and outside == ON and inside == OFF:
          state2 = 3
        elif state2 == 2 and outside == ON and inside == OFF:
          state2 = 3
        elif (state2 == 2 or state2 == 3) and outside == OFF and inside == OFF:
          count -= 1
          print count, "people here"
          if os.path.exists('count.txt'):
            os.remove('count.txt')

          f = open('count.txt', 'w')
          f.write(str(count))
          f.close()
          state2 = 0
  prevDoor = door

f.close()
