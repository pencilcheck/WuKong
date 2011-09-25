require 'serialport'

Board = Struct.new :board_id, :baud, :type, :sink_address, :location
Sensor = Struct.new :sensor_id, :type, :pin, :interval, :sensitivity, :address, :mode, :status

$sp = SerialPort.new ARGV[0], 9600
puts $sp.readline # starting up

def describe(board_id, sensors)
end

def insert(board_id, sensors)
  $sp.write(board_id + " insert " + sensors.map {|sensor| sensor.members.select {|name| not sensor[name].nil?}.map {|name| name.to_s + ":" + sensor[name].to_s if not sensor[name].nil?}.join ','}.join(' '))

  msg = $sp.readline
  puts msg
  if msg.include? "success"
    msg.split(' ')[3].split(':')[1].split(',').zip(sensors).map do |id, sensor|
      sensor[:sensor_id] = id
    end
  end
end

def update(board_id, sensors)
  $sp.write(board_id + " update " + sensors.map {|sensor| sensor.members.select {|name| not sensor[name].nil?}.map {|name| name.to_s + ":" + sensor[name].to_s if not sensor[name].nil?}.join ','}.join(' '))

  msg = $sp.readline
  if msg.include? "success"
    msg.split(' ')[3].split(':')[1].split(',').zip(sensors).map do |id, sensor|
      # update all members except id
    end
    sensors
  end
end

def delete(board_id, sensors)
end

def disable(board_id, sensors)
end

def read(board_id, sensors)
  puts board_id + " read " + sensors.map {|sensor| sensor.members.select {|name| name === :interval or name === :sensor_id}.map {|name| name.to_s + ":" + sensor[name].to_s}.join ','}.join(' ')
  $sp.write(board_id + " read " + sensors.map {|sensor| sensor.members.select {|name| name === :interval or name === :sensor_id}.map {|name| name.to_s + ":" + sensor[name].to_s}.join ','}.join(' '))

  while 1
    puts $sp.readline
  end
  msg = $sp.readline
  if msg.include? "success"
    msg.split(' ')[3].split(':')[1].split(',').zip(sensors).map do |id, sensor|
      # update all members except id
    end
    sensors
  end
end

def write(board_id, sensors)
end

# Application Logic below
# People counter

sensors = [Sensor.new(nil, "PIR", 3, 10, "500/10", nil, nil, nil), Sensor.new(nil, "PIR", 4, 10, "500/10", nil, nil, nil)]

puts insert("12345", sensors)
puts read("12345", sensors)
