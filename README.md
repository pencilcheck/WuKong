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
Wukong/node.pde
This is the pde file that you should modify and run it on a end point arduino
board that transmit sensor data back to the data sink node

Wukong/sink.pde
This is the pde file that you should modify and run it on a data sink arduino
board that receives data from the end point node

Wukong/client/

