# We use serialport gem to support reading/writing to a serial port
require 'serialport'

# Define a board struct with an id, baud rate, type, sink_address, and location
Board = Struct.new :board_id, :baud, :type, :sink_address, :location

# Define a sensor struct with an id, type, pin, sample rate, sensitivity,
# address, mode, status, and value
Sensor = Struct.new :sensor_id, :type, :pin, :interval, :sensitivity, :address, :mode, :status, :value

$sp = SerialPort.new ARGV[0], 9600
puts $sp.readline # starting up

def describe(board_id, sensors)
end

def insert(board_id, sensors)
  puts board_id + " insert " + sensors.map {|sensor| sensor.members.select {|name| not sensor[name].nil?}.map {|name| name.to_s + ":" + sensor[name].to_s if not sensor[name].nil?}.join ','}.join(' ')
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

sensors = [Sensor.new(nil, "PIR", 3, 1, "500/10", nil, nil, nil, nil), Sensor.new(nil, "PIR", 2, 1, "500/10", nil, nil, nil, nil)]

puts insert("12345", sensors)
puts read("12345", sensors)

count = 0

state1 = :start
state2 = :start

while 1
  read = $sp.readline.split[1..-1]
  #puts read
  if read[0].include? ','
    read[0].split(',').each do |idvalue|
      id = idvalue.split(':')[0]
      value = idvalue.split(':')[1]

      #puts "id:value -> " + id + ":" + value
      sensors.select {|sensor| sensor[:sensor_id] == id}.each {|sensor| sensor[:value] = value.to_i}
    end
  end


  # determine which state machine to use
  if state1 == :start and state2 == :start
    if sensors[0][:value] == 0 and sensors[1][:value] == 1
      state1 = :first
    elsif sensors[1][:value] == 0 and sensors[0][:value] == 1
      state2 = :first
    end
  end

  if state1 != :start
    # 0 to 1
    if state1 == :first and sensors[0][:value] == 0 and sensors[1][:value] == 0
      state1 = :second
    elsif state1 == :second and sensors[0][:value] == 1 and sensors[1][:value] == 0
      state1 = :final
    elsif state1 == :final and sensors[0][:value] == 1 and sensors[1][:value] == 1
      count += 1
      puts "people: " + count.to_s
      state1 = :start
    end
  elsif state2 != :start
    # 1 to 0
    if state2 == :first and sensors[0][:value] == 0 and sensors[1][:value] == 0
      state2 = :second
    elsif state2 == :second and sensors[0][:value] == 0 and sensors[1][:value] == 1
      state2 = :final
    elsif state2 == :final and sensors[0][:value] == 1 and sensors[1][:value] == 1
      count -= 1
      puts "people: " + count.to_s
      state2 = :start
    end
  end
end
