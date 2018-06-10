# UltrasoundLocalPositioningSystem-CEBRIaN
This is the SW of a beacon of an ULPS which have integrated a EMW3162 WiFi Module and it permit to the user modify the modulation parameters accross a webserver 

For perform this project you will need to: 
1. Read the scheme of the project and build the system.
2. Save this project from 0x800C000.
3. Save the bootloader and Broadcom driver (mxchip.com). Althought the bootloader and the driver are factory installed.
4. Turn the SSID and SSID key to your own WLAN.
5. This system is based of TOF (Time Of Flight) of the sent Kasami sequences and for these you need a receptor which perform a ACF(Auto Correlation Function) and CCF(Cross Correlation Function) for calculate the TOF and the position of itself. 
