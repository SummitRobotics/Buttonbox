import serial
import serial.tools.list_ports
import struct
import time
import warnings

from cobs import cobs
from crc import Calculator, Configuration, Crc16

# Figures out which port has the arduino
def get_arduino_port():
    arduino_ports = [
        p.device
        for p in serial.tools.list_ports.comports()
        if 'Arduino' in p.description  # may need tweaking to match new arduinos
    ]
    if not arduino_ports:
        raise IOError("No Arduino found")
    if len(arduino_ports) > 1:
        warnings.warn('Multiple Arduinos found - using the first')
    return arduino_ports[0]

# Connect to arduino
arduino_port = get_arduino_port()
arduino = serial.Serial(port=arduino_port, baudrate=115200, timeout=0)

# Create checksum calc with arduino crc16 polynomial
calculator = Calculator(Configuration(
        width=16,
        polynomial=0x8001
    ))

running_buffer = []
while True:
    if arduino.is_open and arduino.in_waiting > 0:
        in_buffer = [i for i in arduino.read(arduino.in_waiting)]
        for b in in_buffer:
            if b == 0:
                try:
                    packet = cobs.decode(bytes(running_buffer))
                    messageType, sequence, buttonState, encoder0Value, encoder1Value, msgCrc = \
                        struct.unpack('>HHHhhH', packet)

                    payloadCrc = calculator.checksum(bytes(packet[:-2]))
                    if payloadCrc == msgCrc:
                        print(f'0x{messageType:02X}: {sequence} {buttonState} {encoder0Value} {encoder1Value}')
                except:
                    pass
                running_buffer = []
            else:
                # add to running queue
                running_buffer.append(b)
                if len(running_buffer) > 32:
                    running_buffer.pop(0)

    # target 50hz
    time.sleep(0.02)
    