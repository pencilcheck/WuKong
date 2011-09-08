=begin
People count in a room
Need to know if there is people coming in or going out

Need two IR sensors, one near the door and one two steps further inside
both sensors are placed facing parallel to the door

Need two states, one for each sensor

For example, let sensor A be the sensor near the door and sensor B be the sensor two steps in.
and let each state with corresponding letter bind to corresponding sensors

If stateA is set before setting stateB, then we can infer that a person is comming in
If stateB is set before setting stateA, then we can infer that a person is leaving

Take the first derivative and only care about 0 to 1 for each sensor to set states
=end

require "serialport"

sp = SerialPort.new ARGV[0], 9600, 8, 1, SerialPort::NONE
id = 0
count_id = 0
count = 0

def setup
  sp.write "JOIN"
  id = sp.gets

  sp.write "DESCRIBE #{id}"
  puts sp.gets

  sp.write "COUNT #{id}"
end

def loop
  while true do
    value = sp.gets.split
    count++ if value[0].to_i == count_id and value[1].to_i == -1
    count-- if value[0].to_i == count_id and value[1].to_i == 1
  end
end

setup()
loop()

sp.close
