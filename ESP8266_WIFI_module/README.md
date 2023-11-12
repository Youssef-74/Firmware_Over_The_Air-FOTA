## Firmware Over-the-Air (FOTA) ESP8266-01(WiFi module) side

### Features:
1. **Wi-Fi Connection:** The ESP8266-01 connects to the Wi-Fi network with SSID "WE_E7B4C7" and password "m8m28975".
2. **Server Connection:** It establishes a connection to the FOTA server at "fotaserver.freevar.com".
3. **Version Checking:** Periodically, the ESP8266 checks the server for the latest firmware version by fetching the version.txt file from "fotaserver.freevar.com/firmware/version.txt".
4. **UART Communication:** When a request for the firmware version is received via UART from the other microcontroller, it dynamically fetches the latest firmware version from the server and responds with this up-to-date version.
5. **LED Indication:** The status of the Wi-Fi connection is indicated by a blinking LED on GPIO2.

### UART Commands:
1. **GetVersion:** Send "GetVersion" to request the latest firmware version.
2. **GetCRC:** Send "GetCRC" to request the CRC32 checksum of the firmware binary.
3. **GetAppSize:** Send "GetAppSize" to request the size of the application firmware.
4. **DownloadApp:** Send "DownloadApp" to initiate the download of the application firmware.

### CRC Verification:
- Before sending the application firmware, the ESP8266 calculates the CRC32 checksum of the firmware binary.
- It then retrieves the CRC value stored on the server in the crc.txt file at "fotaserver.freevar.com/firmware/{version}/crc.txt".
- The ESP8266 compares the calculated CRC with the server's CRC to ensure data integrity before initiating the firmware transfer.

### Application Transfer:
- After successful CRC verification, the ESP8266 sends the application firmware to the other microcontroller via UART.

### How to Use:
1. Set up the ESP8266-01 with the appropriate connections and power.
2. Program the ESP8266 with the provided Arduino code using the Arduino IDE.
3. Connect to the ESP8266 via UART from the other microcontroller.
4. Initiate communication using one of the supported commands: "GetVersion", "GetCRC", "GetAppSize", or "DownloadApp".

Feel free to customize and extend this project based on your specific requirements.

