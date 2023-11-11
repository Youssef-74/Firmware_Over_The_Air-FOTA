# Firmware Over-The-Air (FOTA) 
<br> Welcome to the Firmware Over-The-Air (FOTA) project! This project is designed to provide a seamless and efficient mechanism for updating firmware on embedded systems. The FOTA system is structured around three main components: the Firmware Update Server, ESP8266-01 acting as a WiFi module, and the Bootloader Application for STM32F103C8 microcontrollers.

# Overview
## Firmware Update Server
<br> The Firmware Update Server serves as the central hub for managing and distributing firmware updates. It facilitates the storage, organization, and distribution of firmware updates to connected devices. The server is responsible for authenticating devices, managing version control, and ensuring the secure and reliable delivery of firmware updates to the ESP8266-01 modules.

## ESP8266-01 WiFi Module
<br> The ESP8266-01 module plays a pivotal role in the FOTA system, serving as the wireless communication bridge between the Firmware Update Server and the STM32F103C8 microcontrollers. It enables devices to connect to the server, request firmware updates, and download and apply the updates wirelessly. The ESP8266-01 ensures a smooth and efficient communication channel, making the firmware update process hassle-free.

## Bootloader Application for STM32F103C8
<br> The Bootloader Application is responsible for managing the firmware update process on the STM32F103C8 microcontrollers. It facilitates the reception, validation, and installation of firmware updates received from the Firmware Update Server via the ESP8266-01 module. The Bootloader ensures the integrity and reliability of the update process while minimizing downtime and disruptions to the connected devices.

### Workflow
<br> - User Uploads Application Update to Server:

Users upload updated versions of the application to the Firmware Update Server.

<br> - ESP8266-01 Waits for Bootloader Signal:

<br> ESP8266-01 modules connected to the STM32F103C8 microcontrollers wait for a signal from the Bootloader to initiate the firmware update process.

<br> - ESP8266-01 Requests Application Version from Server:

<br> Upon receiving the signal, the ESP8266-01 communicates with the Firmware Update Server, requesting information about the latest application version.

<br> - Server Sends Application Version to ESP8266-01:

<br> The Firmware Update Server responds with the version information, and the ESP8266-01 retrieves the necessary firmware update.

<br> - ESP8266-01 Sends Update to Bootloader via UART:

<br> The ESP8266-01 communicates with the Bootloader application on the STM32F103C8 microcontroller via UART, sending the firmware update.

<br> - Bootloader Validates and Installs Update:

<br> The Bootloader validates the received firmware update, ensuring its integrity.
If the update is valid, the Bootloader installs the new firmware on the microcontroller, completing the update process.
System Architecture Diagram


## Documentation
<br> For detailed documentation on each component and its functionalities, refer to the documentation file in each folder in this repository.
