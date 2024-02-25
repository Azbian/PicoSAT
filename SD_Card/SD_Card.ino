  // This code is developed from the Camera_func.ino for OV2640 ArduCAM mini pro and using it with SD card
  //The camera part of the code is allready defined at the Camera_func.ino


#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <SD.h>
#include "memorysaver.h"
  //This demo can only work on OV2640_MINI_2MP_PLUS platform.
#if !(defined OV2640_MINI_2MP_PLUS)
  #error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

    // set pin 7 as the slave select for the digital pot:
const int Cam_CS = 10;
const int SD_CS  = 9;
int   pos        = 0;   //To track the position of the file location
File myFile;  //Initializing File

uint8_t  start_capture = 3;
uint32_t length        = 0, cycles = 0;

#if defined (OV2640_MINI_2MP_PLUS)
  ArduCAM myCAM( OV2640, Cam_CS );
#endif
uint8_t read_fifo_burst(ArduCAM myCAM);


void setup() {

uint8_t vid, pid;
uint8_t temp;
#if defined(__SAM3X8E__)
  Wire1.begin();
  Serial.begin(9600);
#else
  Wire.begin();
  Serial.begin(9600);
#endif

                                                  //SD card initialization

SPI.begin();
if(!SD.begin(SD_CS)){
  Serial.println("SD card not found.");
}
pinMode(Cam_CS, OUTPUT);
digitalWrite(Cam_CS, HIGH);
pinMode(SD_CS, OUTPUT);
digitalWrite(SD_CS, HIGH);
SPI.end();  //SD card SPI end
            //Camera initialization

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
SPI.end();  //Camera initialization end

}
void loop() {
  if (Serial.available()) {
    int     pixels=128;    //Expected pixels bytes number
    byte    buff[pixels];  //Array that contains the pixels
    capture();             //Function for capturing image
    Serial.println("Image capture successful.");
    length = myCAM.read_fifo_length();
    Serial.println(length, DEC);
      //Length of the image is 153608, if matched picture is ok
    if (length == 153608) {
      cycles = length / pixels;  //How many cycles for transfer the whole image
      Serial.print("Cycles: ");
      Serial.print(cycles);
      Serial.println();
      Serial.write("str");  //Indicate the start of the image
      for (int i = 0; i < cycles; i++) {
        read_img(pixels, buff);  //Read the image data from the FIFO of the camera
        Serial.println("Have ead the file");
        write_to_SD("img.txt",buff,pixels);  //Function to write the buffer to the SD card
        Serial.print("Success fully written the 128 bytes to the SD card at cycle");
        Serial.print(i);
        Serial.println();
      }
    }
    myCAM.clear_fifo_flag();
    Serial.println("Writing done!!!!");
    delay(2000);
    return;
  }

}

  //Function to write epecific amount of bytes at SD card
void write_to_SD(char filename[], char data[], int len){
  SPI.begin();
  myFile = SD.open(filename,FILE_WRITE);
  if(myFile){
    Serial.println("Opening file successful.");
  }
  myFile.seek(myFile.size());
  for(int i=0;i<len;i++){
    myFile.write(data[i]);
  }
  myFile.flush();
  delay(100);
  myFile.close();
  SPI.end();
}

  //Function to capture image
void capture(){
  myCAM.OV2640_set_JPEG_size(OV2640_320x240);delay(1000);
  myCAM.OV2640_set_Light_Mode(Auto);
  if (start_capture == 3)
  {
      //Flush the FIFO
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    delay(200);
      //Start capture
    myCAM.start_capture();
    start_capture = 0;
    delay(200);
  }
}

  //Function to read image from FIFO chip of AdcuCAM

byte read_img(int pixels, byte buff[]) {
  if (length == 153608) {

      //    Serial.println(F("ACK CMD CAM Capture Done. END")); delay(50);
    if (length >= MAX_FIFO_SIZE )
    {
        //      Serial.println(F("ACK CMD Over size. END"));
      myCAM.clear_fifo_flag();
      return;
    }
    if (length == 0 )  //0 kb
    {
        //      Serial.println(F("ACK CMD Size is 0. END"));
      myCAM.clear_fifo_flag();
      return;
    }
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();  //Set fifo burst mode
    byte VH, VL;
    int count = 0;
    for (int i = 0; i < (pixels/2); i++)
    {
      VH          = SPI.transfer(0x00);;
      VL          = SPI.transfer(0x00);;
      buff[count] = VL;
      count++;
      buff[count] = VH;
      count++;
    }
      //    Serial.write("ent");
    myCAM.CS_HIGH();
      //Clear the capture done flag
  }
}

  //Function to check the file size
int FILE_SIZE(char filename[]) {
  int file_size = 0;
  SPI.begin();
  myFile    = SD.open(filename, FILE_READ);
  file_size = myFile.size();
  myFile.close();
  SPI.end();
  return file_size;
}

  //Function to read epecific amount of bytes from SD card
void SD_read(char filename[], int data_len, char data[]) {
  SPI.begin();
  myFile = SD.open(filename, FILE_READ);
  if (myFile) {
    myFile.seek(pos);
    myFile.read(data, data_len);
    pos += data_len;
    myFile.close();
  } else {
    Serial.println("!!Error opening the file!!");
  }
  SPI.end();

}
