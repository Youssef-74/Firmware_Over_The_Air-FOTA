# FOTA Bootloader Application side

Welcome to the Firmware Over-The-Air (FOTA) Bootloader Application for STM32F103C8 microcontrollers. This bootloader is an integral part of the FOTA project, facilitating seamless firmware updates on embedded systems.

## Flash Partitioning

The flash memory on the STM32F103C8 microcontroller is divided into three partitions:

1. **Bootloader Partition:**
   - Size: 20 KB
   - Starting Address: 0x08000000

2. **Non-Volatile Variables Space:**
   - Size: 1 KB
   - Starting Address: 0x08005000

3. **Application Partitions:**
   - Application One
     - Size: 15 KB
     - Starting Address: 0x08005400
   - Application Two
     - Size: 25 KB
     - Starting Address: 0x08009000

## Mechanism

### Bootloader Execution without Uploaded Application

If there is no uploaded application on the server, the bootloader will keep executing itself. It accomplishes this by repeatedly asking the server for the version. In this case, the bootloader returns version "0.0.0" to indicate that there is no application available on the server.

### Bootloader Execution with Uploaded Application

When an application is uploaded to the server, the bootloader will:

1. **Request Application Version and File Size from Server:**
   - The bootloader communicates with the server to request the version and file size of the available application.

2. **Allocate Dynamic Space in SRAM:**
   - The bootloader dynamically allocates space in SRAM based on the file size received from the server.

3. **Download Application into SRAM:**
   - Upon receiving version and file size information, the bootloader downloads the corresponding application into the allocated space in STM32F103C8's SRAM.

4. **CRC Check:**
   - The bootloader performs a CRC check on the downloaded application to ensure its integrity.

5. **Compare Calculated CRC with CRC from Server:**
   - The calculated CRC is compared with the CRC received from the server to ensure the accuracy of the received data.

6. **Write Application to Flash:**
   - If the CRC check is successful, the bootloader writes the application to the appropriate flash partition.

7. **Jump to Executable Application:**
   - After writing the application to flash, the bootloader deletes the dynamic array used for storing the application in SRAM, deinitializes itself, and jumps to the executable application.

### Non-Volatile Variables Space

The bootloader utilizes the non-volatile variables space to store essential information:

- **Version of the Running Application**
- **Number of the Running Application (One or Two)**
- **Indicator Variable for Bootloader Initialization Status**

These variables are crucial for maintaining state information and ensuring a smooth transition between the bootloader and the application.

## Getting Started

To integrate the FOTA Bootloader into your project, follow the instructions provided in the `Bootloader` folder. Review the documentation for configuration settings, API usage, and customization options.

Feel free to explore and contribute to the development of the FOTA Bootloader for STM32F103C8. We welcome feedback, bug reports, and contributions to enhance the functionality and reliability of the bootloader.

Happy coding!

[Your Organization Name]

