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

require 'rubygems'
require 'serialport'
require 'test/unit'


#sp = SerialPort.new ARGV[0], 9600, 8, 1, SerialPort::NONE
#sp = SerialPort.new ARGV[0], 9600
#sp = SerialPort.new "/dev/tty.usbmodem621", 115200

#sp.write "12345 insert pin:4,type:Motion,sensitivity:50/10"
=begin
while true
  puts sp.readline
end
=end

$sp = SerialPort.new ARGV[0], 9600
puts $sp.readline

# test cases
class WuKongTests < Test::Unit::TestCase
  def test_success_message_for_inserting_one_new_sensor
    $sp.puts "12345 insert pin:4,type:Motion,sensitivity:50/10"
    value = $sp.readline.chomp
    puts value
    assert_equal "success 12345 insert sensor_id:0", value
  end

  def test_success_message_for_writing_one_sensor
    $sp.puts "12345 write sensor_id:0,set_value:1"
    value = $sp.readline.chomp
    puts value
    assert_equal "success 12345 write sensor_id:0", value
  end
=begin
  def test_success_message_for_reading_one_sensor
    $sp.puts "12345 read sensor_id:0,interval:1000"
    value = $sp.readline.chomp
    puts value
    assert_equal "success 12345 read sensor_id:0", value
  end
=end
end
