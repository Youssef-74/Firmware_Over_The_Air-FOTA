/*
 * Bootloader_interface.h
 *
 *  Created on: Oct 29, 2023
 *      Author: joe
 */

#ifndef BOOTLOADER_INTERFACE_H_
#define BOOTLOADER_INTERFACE_H_


struct Server {

	uint8_t MAJOR_Version;
	uint8_t MINOR_Version;
	uint8_t PATCH_Version;
	uint16_t file_size;
	uint32_t file_CRC;
};

extern struct Server DataFromServer;

void Bootloader_Init(void);
void Bootloader_Start(void);

#endif /* BOOTLOADER_INTERFACE_H_ */
