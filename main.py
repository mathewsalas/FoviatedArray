import wave
import time
import serial
import keyboard
import numpy as np

file_name = "test.wav"
wf = wave.open(file_name, "w")
sample_rate = 50000
bit_depth = 2  # 24-bit audio, represented in 3 bytes

wf.setnchannels(1)
wf.setsampwidth(bit_depth)
wf.setframerate(sample_rate)

arduino = serial.Serial(port='COM9', baudrate=2000000, timeout=None)
arduino.setDTR(False)
time.sleep(1)
arduino.flushInput()
arduino.setDTR(True)
time.sleep(3)

buffer_size = 1600  # Adjust buffer size as needed
data_buffer = bytearray()

print('Starting collection...')
arduino.write(b'A')
start_time = time.time()


while True:
    # Read bytes into the buffer
    channel_data = arduino.read(buffer_size)

    # Convert the buffer data from bytes to a NumPy array
    samples = np.frombuffer(channel_data, dtype=np.uint8)
    print(len(samples))

    # Reconstruct the 12-bit unsigned values from the bytes
    unsigned_samples = np.zeros(len(samples) // 2, dtype=np.uint16)
    unsigned_samples = (samples[::2].astype(np.uint16) + (samples[1::2].astype(np.uint16) << 8)) & 0x0FFF

    # Convert the unsigned 12-bit samples to signed 16-bit representation
    signed_samples = (unsigned_samples.astype(np.int16) - 2048) * 8

    # Write the signed samples to the WAV file
    wf.writeframes(signed_samples.tobytes())
    print(f'Collected {wf.getnframes()} samples', end='\r')

    if keyboard.is_pressed("k"):
        break

end_time = time.time()
collection_time = end_time - start_time

total_bits_received = wf.getnframes() * 2 * 8
sample_rate = wf.getnframes() / collection_time
data_rate = total_bits_received / collection_time

print('\nCollection finished')
print(f'Total time: {collection_time:.2f} seconds')
print(f'Samples rate: {sample_rate:.2f} Samples per second')
print(f'Data rate: {data_rate:.2f} bits per second')

wf.close()
