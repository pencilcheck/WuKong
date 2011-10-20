import serial
from sys import argv

sp = serial.Serial(argv[1], 9600)
sp.open()

#sp.write("12345 insert pin:4,type:Motion,sensitivity:50/10\n")
while True:
  print sp.readline()
