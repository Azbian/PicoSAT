from ctypes import sizeof
import serial
import io
import sys
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

ser=serial.Serial('COM3',9600, timeout=0.5)  #74880 is the Highest baud rate

def rd():
    count=0
    buffer = bytearray()
    data=bytearray()
    flag=0
    while ser.readable():
        b=ser.read() 
        buffer.extend(b)
        if flag==0:
            print(b)
        if flag:
            count=count+1
            print(count)  
            
            data.extend(b)
            if count==153600:
                ser.close()
                print(f"Image size:{count}")
                return data
                break
        if buffer[-3:]==b"str":
            flag=1
        if buffer[-4:]==b"give":
            give=input("Give serial input:")
            ser.write(int(give).to_bytes(1, byteorder='big'))

def convert_bgr565_to_rgb(bgr565_array):

  # Split the BGR565 pixels into their red, green, and blue components.
  red_array = (bgr565_array >> 11) & 0x1F
  green_array = (bgr565_array >> 5) & 0x3F
  blue_array = bgr565_array & 0x1F

  # Scale the red, green, and blue components to the range [0, 255].
  red_array *= 8
  green_array *= 4
  blue_array *= 8

  # Combine the red, green, and blue components into an RGB pixel array.
  rgb_array = np.stack([red_array, green_array, blue_array], axis=2)

  return rgb_array


def plot_image_from_pixel_buffer(pixel_buffer, width, height, bits_per_pixel):
  # Convert the pixel buffer to a NumPy array.
  if bits_per_pixel == 16:
    image_array =np.flip(np.flip(np.frombuffer(pixel_buffer, dtype=np.uint16).reshape((height, width))),axis=1)
  else:
    raise ValueError("Unsupported bits per pixel: {}".format(bits_per_pixel))

  # Plot the image.
  plt.imshow(convert_bgr565_to_rgb(image_array))
  plt.show()

buf=rd()
print(len(buf))
plot_image_from_pixel_buffer(buf,320,240, 16)