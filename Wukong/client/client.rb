require 'serialport'

$sp = SerialPort.new ARGV[0], 9600

while true
  puts $sp.readline
end
