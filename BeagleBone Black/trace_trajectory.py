#!/usr/bin/python

import serial

# Beaglebone reads data from the Arduino Detector through USB
serial_port = serial.Serial(port="/dev/ttyUSB0", baudrate=9600, timeout=1)

x_pos = 0
y_pos = 0

with open("x.txt", "w") as x, open("y.txt", "w") as y:

	x.write(str(x_pos) + ",")
	y.write(str(y_pos) + ",")

	while True:

    # If data is coming through
		if serial_port.isOpen():
      
			val = serial_port.read()

      # Read the data, increment the positions and write it to the output files
			if val == 'f' or val == 'b' or val == 'r' or val == 'l':
		
				if val == 'f':
					y_pos += 1

				if val == 'b':
					y_pos -= 1
		
				if val == 'r':
					x_pos += 1

				if val == 'l':
					x_pos -= 1
	
				x.write(str(x_pos) + ",")
				y.write(str(y_pos) + ",")

        
