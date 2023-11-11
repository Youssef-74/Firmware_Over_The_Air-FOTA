# Firmware Over-The-Air (FOTA) 
<br> Welcome to the Firmware Over-The-Air (FOTA) project! This project is designed to provide a seamless and efficient mechanism for updating firmware on embedded systems. The FOTA system is structured around three main components: the Firmware Update Server, ESP8266-01 acting as a WiFi module, and the Bootloader Application for STM32F103C8 microcontrollers.

# Overview
## Firmware Update Server
<br> The Firmware Update Server serves as the central hub for managing and distributing firmware updates. It facilitates the storage, organization, and distribution of firmware updates to connected devices. The server is responsible for authenticating devices, managing version control, and ensuring the secure and reliable delivery of firmware updates to the ESP8266-01 modules.

## ESP8266-01 WiFi Module
<br> The ESP8266-01 module plays a pivotal role in the FOTA system, serving as the wireless communication bridge between the Firmware Update Server and the STM32F103C8 microcontrollers. It enables devices to connect to the server, request firmware updates, and download and apply the updates wirelessly. The ESP8266-01 ensures a smooth and efficient communication channel, making the firmware update process hassle-free.

## Bootloader Application for STM32F103C8
<br> The Bootloader Application is responsible for managing the firmware update process on the STM32F103C8 microcontrollers. It facilitates the reception, validation, and installation of firmware updates received from the Firmware Update Server via the ESP8266-01 module. The Bootloader ensures the integrity and reliability of the update process while minimizing downtime and disruptions to the connected devices.

## Documentation
<br> For detailed documentation on each component and its functionalities, refer to the documentation file in each folder in this repository.
