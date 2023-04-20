How upload code to the M5
========================

1. Install VSCode: https://code.visualstudio.com/download
2. Install M5stack drivers: https://docs.m5stack.com/en/quick_start/m5core/arduino (Dunno which one you need, I installed both)
3. Install the github desktop app: https://desktop.github.com/ 
4. Clone the repo "5080-Team-Project" 
5. Install PlatformIO in VSCode
6. Open main.cpp, set no_tca to false. 
7. Upload to M5 by pressing -> on the bottom left of VSCode (M5 should be plugged in)
8. There will be errors, because the name of the upload port is different for every pc. 
9. Open the platformio.ini file, and change the upload_port to the correct port.
10. Only method I know to check the name of the port is to install the Arduino IDE (https://www.arduino.cc/en/software), click Select Board, and then select the M5Stack-Core-ESP32. It will show you the port and serial number of the plugged in M5 (see uploaded screenshot). Maybe you can do this from the device manager, but I haven't tried this on windows.

Measurement results 
========================

Battery measurement 1: 3hrs 45min

Battery measurement 2: 3hrs 49min 

Average battery life:  3hrs 47min 

% of successfully received packets was measured by sending one character via serial each time the buffer was sent via BLE. This was done to minimise the effect of the serial communication to the loop, as each sent character takes time, therefore sending the whole measurement string would slow down the loop. The time of sending one character via serial is negligible. The number of times the character was received was counted, and it was used to divide the number of packets received via BLE. This measurement was 30 minutes long. 

Number of packets received via BLE: 178,287

Number of characters received via serial: 179,401 

Percentage of successfully received packets: 99.93%

Average frequency: 99.67Hz 

Frequency file from which the frequency graph was made is found in this repo. 

Range was measured to be more than 15 metres. 