WuKong Intelli-flow
========================
Intelli-flow is one of several sub projects under Wukong
It is intended to implement a People Flow monitoring application to monitor
people flow in a room with various kinds of sensors including
IR switch sensors, and motion sensors and more ...

How to run
----------
Open the pde files using Arduino IDE and upload it to the corresponding board to get started
node.pde: To the sensor board
sink.pde: To the data sink board

Framework structure
-------------------
libraries/Xbee
This is the xbee library for arduino. This project is hosted on google code on
http://code.google.com/p/xbee-arduino/

Wukong/node.pde
This is the pde file that you should modify and run it on a end point arduino
board that transmit sensor data back to the data sink node

Wukong/sink.pde
This is the pde file that you should modify and run it on a data sink arduino
board that receives data from the end point node

Wukong/client/people\_counter
This is where the script that one should run on the server that retrieve data
from the data sink node via serial port and process them. It will write the
state of each variable to a file. The counter variable is written to the
count.txt and door variable is written to door.txt

Wukong/client/apache
This is the folder that contains the client code files that are placed in apache www folder
One should install mod\_wsgi before proceeding to install the code
The client basically implements a webpage on client that does a semi-long poll
to the server for updating state variables, such as door state and people count
number. The webpage is updated in almost real time.
