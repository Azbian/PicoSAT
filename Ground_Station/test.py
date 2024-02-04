import ephem
import urllib.request
import time
import serial
import os
import datetime
from sgp4.api import Satrec
# Latitude and longitude of he observer (in degrees)
#BSMRAAU
observer_lat = '25.9023522'   
observer_lon = '89.4325258'
# User input and change that name according to URL
satellite_name= input("Enter the name of the CUBESAT (e.g. CUTE-1 (CO-55)): ")
satellite=satellite_name
i=satellite.find(' ')
if i>0:
    satellite=satellite[:i]+"%20"+satellite[i+1:]
# URL for the TLE data
url = f'https://celestrak.org/NORAD/elements/gp.php?NAME={satellite}&FORMAT=tle'

# Create an ephem observer object for the observer's location
observer = ephem.Observer()
observer.lat = ephem.degrees(observer_lat)
observer.lon = ephem.degrees(observer_lon)
# Fetch TLE data
req = urllib.request.urlopen(url)
TLEdata = req.read().decode("utf-8")
lines = TLEdata.strip().split('\n')

while True:
    # Create an ephem satellite object
    satellite = ephem.readtle(lines[0], lines[1], lines[2])

    # Set the observer's date and time to the current time
    observer.date = ephem.now()

    # Compute the satellite's position
    satellite.compute(observer)

    # Get the azimuth and elevation angles and convert them radian to degree
    azimuth = satellite.az * 180 / ephem.pi
    elevation = satellite.alt * 180 / ephem.pi
    sat=Satrec.twoline2rv(lines[0],lines[1])
    ps,pe=Satrec.next_pass(Observer(observer_lat,observer_lon,0))
    print(ps)


    # Print the angles
    os.system('cls')
    print("Satellite : "+satellite_name)
    print("Real time : "+str(ephem.localtime(ephem.now())))
    print("Estimated time : "+str(ephem.localtime(observer.next_pass(satellite)[0])))
    print(f"Azimuth: {azimuth:.2f}°, Elevation: {elevation:.2f}°")
    time.sleep(0.5)