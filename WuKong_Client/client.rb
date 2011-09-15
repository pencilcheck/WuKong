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
#sp = SerialPort.new ARGV[0], 9600
puts "Done initialization"
count = 0

=begin
puts "write JOIN"
sp.write "JOIN"
id = sp.read
puts "ID:", id

puts "write DESCRIBE"
sp.write "DESCRIBE #{id}"
puts sp.read
=end

puts "write COUNT"
sp.puts 'C'

while true
  value = sp.gets.to_i
  if value == 1
    count += 1
    puts "#{count} people in the room"
  end
end
