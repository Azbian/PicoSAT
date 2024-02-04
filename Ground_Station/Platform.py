import ephem
import urllib.request
import time
import serial
import os

# Define the serial port
ser = serial.Serial('COM3',  baudrate=115200, timeout=.1)

# Latitude and longitude of he observer (in degrees)
#BSMRAAU
observer_lat = '25.9023522'   
observer_lon = '89.4325258'

#Dhaka
#observer_lat = '23.7131986'  
#observer_lon = '90.4016137'

#False location
#observer_lat = '347.143'  
#observer_lon = '266.63'

# User input and change that name according to URL
sat_name= input("Enter the name of the CUBESAT (e.g. CUTE-1 (CO-55)): ")
sat=sat_name
i=sat.find(' ')
if i>0:
    sat=sat[:i]+"%20"+sat[i+1:]
# URL for the TLE data
url = f'https://celestrak.org/NORAD/elements/gp.php?NAME={sat}&FORMAT=tle'

# Create an ephem observer object for the observer's location
observer = ephem.Observer()
observer.lat = ephem.degrees(observer_lat)
observer.lon = ephem.degrees(observer_lon)

# Fetch TLE data
req = urllib.request.urlopen(url)
TLEdata = req.read().decode("utf-8")
lines = TLEdata.strip().split('\n')



while True:
    # Create an ephem sat object
    sat = ephem.readtle(lines[0], lines[1], lines[2])

    # Set the observer's date and time to the current time
    observer.date = ephem.now()

    # Compute the sat's position
    sat.compute(observer)

    # Get the azimuth and elevation angles and convert them radian to degree
    azimuth = sat.az * 180 / ephem.pi
    elevation = sat.alt * 180 / ephem.pi

    # Print the angles
    os.system('cls')
    print("Real time : "+str(ephem.localtime(ephem.now())))
    print("Estimated time : "+str(ephem.localtime(observer.next_pass(sat)[0])-ephem.localtime(ephem.now())))
    print(f"Azimuth: {azimuth:.2f}°, Elevation: {elevation:.2f}°")
    data = f'{azimuth:.4f},{elevation:.4f}'
    # Send angles through 'COM' port 
    if not ser.isOpen():
        ser.open()
        ser.write(data.encode())
        ser.close()
    else:
        print("Arduino is not connected.")
    time.sleep(0.5)