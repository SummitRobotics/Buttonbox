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

class PacketDecoder:
    def __init__(self, max_buffer_size=128):
        self.running_buffer = []
        self.max_buffer_size = max_buffer_size

    def consume(self, buffer: bytes):
        packets = []
        for b in buffer:
            if b == 0:
                try:
                    packet = cobs.decode(bytes(self.running_buffer))
                    packets.append(packet)
                except:
                    # failed to decode (could have just connected or maybe invalid data)
                    pass
                self.running_buffer = []
            else:
                # add to running queue
                self.running_buffer.append(b)
                if len(self.running_buffer) > self.max_buffer_size:
                    self.running_buffer.pop(0)
        return packets

decoder = PacketDecoder()
while True:
    if arduino.is_open and arduino.in_waiting > 0:
        packets = decoder.consume(arduino.read(arduino.in_waiting))
        for packet in packets:
            try:
                # unpack the message
                messageType, sequence, buttonState, encoder0Value, encoder1Value, msgCrc = \
                    struct.unpack('>HHHhhH', packet)

                # confirm CRC matches payload
                payloadCrc = calculator.checksum(bytes(packet[:-2]))
                if payloadCrc == msgCrc:
                    # TODO got an ButtonReportMsg; process it
                    print(f'0x{messageType:02X}: {sequence} {buttonState} {encoder0Value} {encoder1Value}')
            except:
                # failed to decode the message
                pass

    # target 50hz
    time.sleep(0.02)
