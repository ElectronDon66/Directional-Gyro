This directional gyro (DG) sensor utilizes the BNO085 gyro sensors and the accelerometer sensors (for sensing down). It does not use the magnetometer sensors. The idea with this project was to make a non-magnetic direction sensor. This can be used where there is steel or iron nearby and also in close proximity to electric motors. The Microcontroller is a Teensy 4.0 by PJRC. The display is a 1.28 inch round TFT LCD  that is designed to look like a typical aircraft DG heading indicator. After power up the compass rose will show “N” and all angular rotations will be relative to that, or you can use the quadrature encoder to set the TFT display heading to that of a magnetic compass if you wish to track magnetic direction. That’s how it’s done in aircraft. As the sensor rotates the compass rose rotates and the direction indicator does not, which is how an aircraft DG functions.  
The Sketch uses the TFT_sSPI graphics library to draw the display and makes use of a sprite for fast no flicker display updates. This library will require you to configure the user setup file and you’ll also need to edit the GC9A01 graphics driver to  set the display pins. They are set in the GC9A01_DRIVER.h  file. After you install the TFT_eSPI library you will need to go into the User_Setup_Select.h  file with a text editor and set the  graphics  display to Setup200_GC9A01.h  and then edit the Setup200_GC9A01.h  to configure  the SPI pins to match this Sketch. For step by step instructions see GraphicsSetupInstructions.txt. 
The BNO085 update rate is 200Hz but the display only redraws if the heading has changed by more than 0.5 degrees. This helps eliminate noise and constantly redrawing the compass rose. The maximum rate of rotation is +/- 2000 degrees/sec. The gyroscope is 16 bit and there is a 32 bit ARM CORTEX processor  that is running software by CEVA called Motion Engine, but this is all invisible to the user. It’s handling reading the gyro registers and producing  the quaternion results for direction. The Teensy then computes the yaw (heading) based on the w,x,y and z quaternion rotation vector. No special calibration is needed. I’ve left the DG turned on for days and essentially observed 0 degrees drift  which surprised me. 
The BNO085 uses the Teensy’s 4.0 I2C bus and the display uses the Teensy SPI bus. (See the hookup jpg drawing). The GY-BNO08X board I purchased stated the default  IC2 address was 0x4A but I found it was actually 0x4B. The Adafruit_BNO08x.h library requires it be 0x4A so I had to ground the AD0 pin to make it 0x4A. If you use the Adafruit BNO085 board you won’t have to ground the address pin. The  breadboard diagram shows the Adafruit part. I couldn’t find a Fritzing part for the GY- BNO08x part so be aware of the address issue depending on the part you buy. You will also need to use the following libraries :
Wire.h
SPI.h
TFT_eSPI.h
Adafruit_BNO08x.h

The youtube video is available at https://youtu.be/UoS7PKGJVlE 






