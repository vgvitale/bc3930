## Short Description  
ESP32 equipped with DHT11 temperature+humidity sensor that, when powered on and connected to the internet, will send temperature and humidity data over WiFi to a Google Script as parameters at specified intervals. The script will then input parameters into a Google Sheet for live, remote access.
## Key Information:  
#### Reproducibility  
**Hardware Requirements:**  
1. ESP32 (TTGO T1 for this instance)
2. DHT11 (for extreme cold or hot climates or if a more frequent sampling rate is desired: use DHT22)
3. ESP32 expansion board (optional)
4. Breadboard
5. 3 male-to-male jumper wires
6. Enclosure (optional but recommended for portability)
#### Installation  
1. In Arduino Library manager, install DHT sensor library (this will allow inclusion of DHT.h)
2. Create a public Google Sheet to save temperature and humidity data.  In the sheet, select Extensions > Apps Script and create a Google Script that will automatically link to active sheet
4. Copy google_script.js into your newly created Google Script, then deploy as a web application
5. Copy module2.ino code into Arduino. Replace WiFi ssid and password with your own. Replace Google Script API link with your own
6. Place ESP32 (with headers) into expansion board or breadboard. Place DHT11 into breadboard. With waffle-pattern facing towards you, connect 3V to left-most pin with jumper wire. Repeat by connecting IO2 (alternatively: change DHTPIN in code) to second-to-left-most pin, and GND to right-most pin
#### Usage  
TODO
