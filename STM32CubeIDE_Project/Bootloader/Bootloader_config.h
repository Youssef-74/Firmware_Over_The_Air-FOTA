/*
 * Bootloader_config.h
 *
 *  Created on: Oct 29, 2023
 *      Author: joe
 */

#ifndef BOOTLOADER_CONFIG_H_
#define BOOTLOADER_CONFIG_H_

#define HAL_UART_HANDLE		 	     &huart1
/**/
#define TxBuffer_size			     2
#define RxBuffer_size				 256
/*		Flash memory partitions		*/
#define BOOTLOADER_ADDRESS_VARIABLES            0x08005000 /**< Flash memory address for flag storage (1 KB). */
#define BOOTLOADER_ADDRESS_BANK_1               0x08005400 /**< Flash memory address for the first application bank (15 KB). */
#define BOOTLOADER_ADDRESS_BANK_2               0x08009000 /**< Flash memory address for the second application bank (25 KB). */
/*		STM32f103c8 SRAM & Flash memories limits and dimensions		 */
#define STM32F103_SRAM_SIZE         (20 * 1024)
#define STM32F103_FLASH_SIZE        (64 * 1024)
#define STM32F103_SRAM_END          (SRAM_BASE + STM32F103_SRAM_SIZE)
#define STM32F103_FLASH_END         (FLASH_BASE + STM32F103_FLASH_SIZE)

#define HAL_SUCCESSFUL_ERASE         0xFFFFFFFFU

/*	 what is the CPU executing now	 */
#define BOOTLOADER_APP				 0x0000	/*	Application of bootloader starting from address 0x0800 0000 with size = 20K	*/
#define APPLICATION_ONE				 0x0001 	/*	Application number one starting from address 0x0800 5400 with size = 15K	*/
#define APPLICATION_TWO				 0x0002	/*	Application number two starting from address 0x0800 9000 with size = 25K	*/
#define NON_VOLATILE_PART			 0x0003 	/*	Application number one starting from address 0x0800 5400 with size = 15K	*/

/* initial version */
#define INIT_MAJOR					0x0000
#define INIT_MINOR					0x0000
#define INIT_PATCH					0x0000
/*	*/
#define CRC_DIGITS_BASE				1000000000
#define CRC_NUMBER_OF_DIGITS		10

// packet length
#define PACKET_LENGTH				256

#endif /* BOOTLOADER_CONFIG_H_ */
