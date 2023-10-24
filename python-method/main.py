#!/usr/bin/env python3
#
# This is a NetworkTables client (eg, the DriverStation/coprocessor side).
# You need to tell it the IP address of the NetworkTables server (the
# robot or simulator).
#
# When running, this will continue incrementing the value 'dsTime', and the
# value should be visible to other networktables clients and the robot.
#

import sys
import time
from networktables import NetworkTables
import serial 

# To see messages from networktables, you must setup logging
import logging

arduino = serial.Serial(port='COM9', baudrate=115200, timeout=.1) 
def write_read(x): 
    #    arduino.write(bytes(x, 'utf-8')) 
    #    time.sleep(0.05)
    data = arduino.read() 
    return data 

logging.basicConfig(level=logging.DEBUG)

if len(sys.argv) != 2:
    print("Error: specify an IP to connect to!")
    exit(0)

ip = sys.argv[1]

NetworkTables.initialize(server=ip)

sd = NetworkTables.getTable("SmartDashboard")
fmsinfo = NetworkTables.getTable("FMSInfo")

i = 0
while True:
    # Example reading from network table
    # print("robotTime:", sd.getNumber("robotTime", -1))
    # print("StationNumber:", fmsinfo.getNumber("StationNumber", -1))

    # Example publishing to network tables
    sd.putNumber("dsTime", i)
    i += 1

    # Read the serial button state and log to console
    value = write_read(str(i)) 
    print(value) # printing the value 

    time.sleep(0.05)

