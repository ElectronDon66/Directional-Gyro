Make these modifications to the TFT_eSPI driver

Use the following graphics driver :
TFT_eSPI library by Bodmer v2.5.43

1. In the Arduino Library manager find and install TFT_eSPI_ES32Lab (by Bodmer). Although its optimized for ESP32 boards it'll work fine with the Teensy 4.0

Of course it is crucial to also properly set two additional files in the  TFT_eSPI library folder.  

2. In Arduino library folder find  User_Setups_Select.h and open it with a text editor such as Notepad.
3. Find the line with #include <User_Setup.h> . Change it to //#include <User_Setup.h> (comment it out)
4. Find the line with //#include <User_Setups/Setup200_GC9A01.h>   and change it to #include <User_Setups/Setup200_GC9A01.h>   (uncomment it)
5. Make sure no other graphics drivers are enabled if so comment them out //
6. Save the file back to the same folder 
7. Next go back the Arduino\libraries\TFT_eSPI  folder and find the User_Setups folder open it and find TFT_eSPI\User_Setups\Setup200_GC9A01.h"
8. Open this file Look for the SPI mapping 
9. Change the defines to look like the mapping below:

#define TFT_MOSI 11 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_SCLK 13
#define TFT_CS   4  // Chip select control pin
#define TFT_DC   9  // Data Command control pin
#define TFT_RST  8  // Reset pin (could connect to Arduino RESET pin)
//#define TFT_BL   22  // LED back-light

Leave everything else alone!! 
10. Save the file back to the same location 

You should now be able to compile the Directional_Gyro.ino without errors. 

If you need more help modifying the TFT_eSPI files this video shows how to do it :
https://www.youtube.com/watch?v=6lLlKY5U77w

