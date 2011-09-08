require "serialport"

sp = SerialPort.new ARGV[0], 9600, 8, 1, SerialPort::NONE
count = 0

while true do
  output = sp.gets
  sp.write ""
end

sp.close
