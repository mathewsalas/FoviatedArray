import wave
import time
import numpy as np
import serial
import keyboard
from bitstring import BitArray

print('opening file...')
file_name = "test.wav"
wf = wave.open(file_name, "w")
print('file open')

sample_rate = 50000

wf.setnchannels(1)
wf.setsampwidth(3)
wf.setframerate(sample_rate)

print('connecting to arduino...')
arduino = serial.Serial(port='COM3', baudrate=2000000)
arduino.setDTR(False)
time.sleep(1)
arduino.flushInput()
arduino.setDTR(True)
time.sleep(3)
print('connected')

print('starting collection...')
arduino.write(b'A')

while True:
    channel = arduino.read(3)
    # print(int.from_bytes(channel, byteorder='big', signed=True), sep=' ')
    wf.writeframes(channel)
    if keyboard.is_pressed("k"):
        break

print('collected ' + str(wf.getnframes()) + ' samples')
print('collection finished')
wf.close()


# for t in np.linspace(0, 5, sample_rate):
#     channel = 0.5 * np.sin(2 * np.pi * 1000 * t)
#     quantized_channel = channel * (2 ** 23 - 1)
#     int_channel = quantized_channel.astype('int').item()
#     wf.writeframes(int_channel.to_bytes(3, byteorder='little', signed=True))
#     print(int_channel, int_channel.to_bytes(3, byteorder='big', signed=True),
#           BitArray(int_channel.to_bytes(3, byteorder='big', signed=True)).bin, sep=' ')
