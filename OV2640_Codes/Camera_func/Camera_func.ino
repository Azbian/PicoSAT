// ArduCAM Mini demo (C)2017 Lee
// Web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with ArduCAM Mini camera, and can run on any Arduino platform.
// This demo was made for ArduCAM_Mini_2MP_Plus.
// It needs to be used in combination with PC software.
// It can test OV2640 functions
// This program requires the ArduCAM V4.0.0 (or later) library and ArduCAM_Mini_2MP_Plus
// and use Arduino IDE 1.6.8 compiler or above
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"
//This demo can only work on OV2640_MINI_2MP_PLUS platform.
#if !(defined OV2640_MINI_2MP_PLUS)
  #error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

// set pin 7 as the slave select for the digital pot:
const int CS = 10;

uint8_t start_capture = 3;
uint32_t length=0,cycles=0;

#if defined (OV2640_MINI_2MP_PLUS)
  ArduCAM myCAM( OV2640, CS );
#endif
uint8_t read_fifo_burst(ArduCAM myCAM);
void setup() {
// put your setup code here, to run once:
uint8_t vid, pid;
uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
  Serial.begin(74880);
#else
  Wire.begin();
  Serial.begin(74880);
#endif

pinMode(CS, OUTPUT);
digitalWrite(CS, HIGH);
SPI.begin();
  //Reset the CPLD
myCAM.write_reg(0x07, 0x80);
delay(100);
myCAM.write_reg(0x07, 0x00);
delay(100);
while(1){
  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55){
    Serial.println(F("ACK CMD SPI interface Error! END"));
    delay(1000);continue;
  }else{
    Serial.println(F("ACK CMD SPI interface OK. END"));break;
  }
}

#if defined (OV2640_MINI_2MP_PLUS)
  while(1){
    //Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
      Serial.println(F("ACK CMD Can't find OV2640 module! END"));
      delay(1000);continue;
    }
    else{
      Serial.println(F("ACK CMD OV2640 detected. END"));break;
    } 
  }
#endif
//Change to JPEG capture mode and initialize the OV5642 module
myCAM.set_format(BMP);
myCAM.InitCAM();
#if defined (OV2640_MINI_2MP_PLUS)
  myCAM.OV2640_set_JPEG_size(OV2640_320x240);
#endif
delay(1000);
myCAM.clear_fifo_flag();
#if !(defined (OV2640_MINI_2MP_PLUS))
myCAM.write_reg(ARDUCHIP_FRAMES,0x00);
#endif
Serial.write("give");
}
void loop() {
  if (Serial.available()) {
    int pixels=128;
    byte buff[pixels];
    capture();
    length = myCAM.read_fifo_length();
    Serial.println(length, DEC);
    if (length == 153608) {
      cycles = length / pixels;
      Serial.println(cycles);
      Serial.write("str");
      for (int i = 0; i < cycles; i++) {
        read_img(pixels, buff);
        for (int j = 0; j < pixels; j++) {
          Serial.write(buff[j]);
        }
      }
    }

    myCAM.clear_fifo_flag();
  }

}
void capture(){
  myCAM.OV2640_set_JPEG_size(OV2640_320x240);delay(1000);
  myCAM.OV2640_set_Light_Mode(Auto);
  if (start_capture == 3)
  {
    //Flush the FIFO
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    //Start capture
    myCAM.start_capture();
    start_capture = 0;
    delay(200);
  }
}

byte read_img(int pixels, byte buff[]) {
  if (length == 153608) {

    //    Serial.println(F("ACK CMD CAM Capture Done. END")); delay(50);
    if (length >= MAX_FIFO_SIZE )
    {
      //      Serial.println(F("ACK CMD Over size. END"));
      myCAM.clear_fifo_flag();
      return;
    }
    if (length == 0 ) //0 kb
    {
      //      Serial.println(F("ACK CMD Size is 0. END"));
      myCAM.clear_fifo_flag();
      return;
    }
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();//Set fifo burst mode
    byte VH, VL;
    int count = 0;
    for (int i = 0; i < (pixels/2); i++)
    {
      VH = SPI.transfer(0x00);;
      VL = SPI.transfer(0x00);;
      buff[count] = VL;
      count++;
      buff[count] = VH;
      count++;
      //      Serial.write(VL);
      //      delayMicroseconds(12);
      //      Serial.write(VH);
      //      delayMicroseconds(12);
    }
    //    Serial.write("ent");
    myCAM.CS_HIGH();
    //Clear the capture done flag
  }
}
