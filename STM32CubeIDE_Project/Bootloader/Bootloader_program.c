/*
 * Bootloader_program.c
 *
 *  Created on: Oct 29, 2023
 *      Author: joe
 */
/******************************  Includes ***********************************/
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "usart.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"

#include "Bootloader_config.h"
#include "Bootloader_private.h"
#include "Bootloader_interface.h"

/******************************  Global Variables Definitions   ***********************************/

uint8_t  UART1_RxBuffer[RxBuffer_size] ;
uint8_t  UART1_TxBuffer[TxBuffer_size] ;

uint16_t volatile DataLength = 0;
uint8_t volatile Iterator = 0  ;
static uint16_t volatile  timeout = 0;

struct Server DataFromServer;
struct FASLH DataFromFlash;

HAL_StatusTypeDef HAL_Status 		= HAL_ERROR;
HAL_StatusTypeDef UART_Status 		= HAL_ERROR;
ESP_StatusTypedef ESP_Status 		= ESP_ERROR;
/* Variables used for Erase pages under interruption*/
FLASH_ProcessTypeDef peFlash;

static uint16_t ReceivingSize ;

unsigned long t_diff, t1, t2;
// pointer to dynamic array in heap to save the received Application from server
uint8_t				*App_In_SRAM ;
static uint16_t		App_Iterator  = 0;
static uint16_t 	Packet_iterator = 0;
/******************************  Software Functions Definitions   ***********************************/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);

	if(ReceivingSize == PACKET_LENGTH)
	{
	   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,1);
		for(Packet_iterator = 0; (Packet_iterator < PACKET_LENGTH) && (App_Iterator < DataFromServer.file_size); Packet_iterator++, App_Iterator++)
		{
			App_In_SRAM[App_Iterator] = UART1_RxBuffer[Packet_iterator];
		}
		/*if(App_Iterator - DataFromServer.file_size < PACKET_LENGTH)
		{
			ReceivingSize = App_Iterator - DataFromServer.file_size;
		}
		else{
			ReceivingSize = PACKET_LENGTH;
		}*/

		if(App_Iterator < DataFromServer.file_size)
		{
			Void_UART_SettingUp();
			HAL_UARTEx_ReceiveToIdle_IT(HAL_UART_HANDLE, (uint8_t*)UART1_RxBuffer, ReceivingSize);
		}
		//t = coreticks() - t;
		 //t2 = DWT->CYCCNT;
		 //t_diff = t2 - t1;
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
	}
}
/* the implementation of callback function of the UART1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	 //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
	 //HAL_Delay(8000);
	 //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);

}

/* UART set-up function to prepare for receiving data */
void Void_UART_SettingUp (void)
{
	memset(UART1_RxBuffer,0,RxBuffer_size);
	//HAL_UART_Receive_IT(HAL_UART_HANDLE, (uint8_t*)UART1_RxBuffer, ReceivingSize);
	HAL_UARTEx_ReceiveToIdle_IT(HAL_UART_HANDLE,UART1_RxBuffer,ReceivingSize);

	// clear ESP8266 Transmitter Buffer
	memset(UART1_TxBuffer,0,TxBuffer_size);
	UART_Status = HAL_ERROR;
	DataLength = 0;
}

/* Function to update the Non-Voltaile Space Variables */
void Bootloader_Update_NONVOLATILE_SPACE(uint16_t App_Number, uint16_t MAJOR, uint16_t MINOR, uint16_t PATCH)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	HAL_Status = HAL_ERROR;
	/* writing into flash memory need to lock the */
	 /* Unlock the FLASH control register access */
	 HAL_Status = HAL_FLASH_Unlock();
	 if(HAL_Status == HAL_OK){
		 /* Process Locked */
		  // __HAL_LOCK(&peFlash);
		 /* Wait for last operation to be completed */
		 status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
		 if(status == HAL_OK)
		 {
			 /* Proceed to program the new data */
			 SET_BIT(FLASH->CR, FLASH_CR_PG);
			 //HAL_Delay(100);
			 /* Write the application number to run equal zero for
			  * Bootloader application as initialization in the NON_VOLATILE_VARIABLE */
			 *(__IO uint16_t*)(&(NON_VOLATILE_VARIABLE.APP_TO_RUN)) = App_Number;
			 /* Wait for last operation to be completed */
			status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
			// write the version number into MAJOR, MINOR, PATCH
			 *(__IO uint16_t*)((NON_VOLATILE_VARIABLE.LATEST_VERSION+0)) = MAJOR;
			 *(__IO uint16_t*)((NON_VOLATILE_VARIABLE.LATEST_VERSION+1)) = MINOR;
			 *(__IO uint16_t*)((NON_VOLATILE_VARIABLE.LATEST_VERSION+2)) = PATCH;

			/* Wait for last operation to be completed */
			status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
			// Check if the variables have already been initialized
			 if (NON_VOLATILE_VARIABLE.initialized != 1) {
		/*  Write the initialization variable in the NON_VOLATILE_VARIABLE equal to ONE */
				 *(__IO uint16_t*)(&(NON_VOLATILE_VARIABLE.initialized)) = 1;
				 /* Wait for last operation to be completed */
				status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
			 }

			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
	 }

   }
	 /* Locks the FLASH control register access */
	HAL_Status = HAL_FLASH_Lock();


}
/*  the implementation of Bootloader Initialization function */
void Bootloader_Init(void)
{
	ESP_Status = ESP_ERROR;

	// Send Wake-Up command to the ESP to be ready
	ESP_Status = Bootloader_Send_Command(ESP_WAKE_UP);

	// Check if the variables have already been initialized
	 if (NON_VOLATILE_VARIABLE.initialized != 1) {
	 // Variables haven't been initialized, set initial values
		 Bootloader_Update_NONVOLATILE_SPACE(BOOTLOADER_APP,INIT_MAJOR,INIT_MINOR,INIT_PATCH);
	 }
	 // take a copy from the NON_VOLATILE_VARIABLEs to be accessed from SRAM faster
	 DataFromFlash.APP_Number    = *(__IO uint16_t*)(&(NON_VOLATILE_VARIABLE.APP_TO_RUN));

	 DataFromFlash.MAJOR_Version = *(__IO uint16_t*)((NON_VOLATILE_VARIABLE.LATEST_VERSION+0));
	 DataFromFlash.MINOR_Version = *(__IO uint16_t*)((NON_VOLATILE_VARIABLE.LATEST_VERSION+1));
	 DataFromFlash.PATCH_Version = *(__IO uint16_t*)((NON_VOLATILE_VARIABLE.LATEST_VERSION+2));


}

void Bootloader_Start(void)
{
	ESP_StatusTypedef		ESP_Status   = ESP_ERROR;
	FLASH_StatusTypedef		Flash_Status = UNSUCCESSFUL_ERASE;
	//Send Wake-up command to the ESPWIFI module
	//ESP_Status = Bootloader_Send_Command(ESP_WAKE_UP);
	//ESP_Status = ESP_ERROR;
	// GET_version command to the ESPWIFI module
	ESP_Status = Bootloader_Send_Command(ESP_GET_VERSION);
	// Receive the version from server if the ESP_Status OK
	if(ESP_Status == ESP_OK)
	{
		Bootloader_Receive_Server_Data(ESP_GET_VERSION);
	}
	ESP_Status = ESP_ERROR;
	//check the stored version in NON_VOLATILE space and compare to the received version from the ESP
	/*in case the version of server is larger than stored in flash
	 * get this update from server and save to flash */
	if((DataFromServer.MAJOR_Version > DataFromFlash.MAJOR_Version)
		|| (DataFromServer.MINOR_Version > DataFromFlash.MINOR_Version)
		|| (DataFromServer.PATCH_Version > DataFromFlash.PATCH_Version))
	{
		// in case of there is a new version of application
		// GET_File_size command to the ESPWIFI module
		ESP_Status = Bootloader_Send_Command(ESP_GET_FILE_SIZE);
		// Receive the File_size from server if the ESP_Status OK
		if(ESP_Status == ESP_OK)
		{
			Bootloader_Receive_Server_Data(ESP_GET_FILE_SIZE);
		}
		ESP_Status = ESP_ERROR;
		// GET_CRC command to the ESPWIFI module
		ESP_Status = Bootloader_Send_Command(ESP_GET_CRC);
		// Receive the CRC from server if the ESP_Status OK
		if(ESP_Status == ESP_OK)
		{
			Bootloader_Receive_Server_Data(ESP_GET_CRC);
		}
		ESP_Status = ESP_ERROR;
		// GET_CRC command to the ESPWIFI module
		ESP_Status = Bootloader_Send_Command(ESP_GET_APP);
		// Receive the CRC from server if the ESP_Status OK
		if(ESP_Status == ESP_OK)
		{
			Bootloader_Receive_Server_Data(ESP_GET_APP);
		}
		ESP_Status = ESP_ERROR;
		HAL_Delay(10000);
		if(App_In_SRAM != NULL)
		{
			uint32_t CRC_Calculated = calculate_crc32(App_In_SRAM, DataFromServer.file_size);
			if(CRC_Calculated == DataFromServer.file_CRC)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,0);
				// in case of we in bootloader application mode (No Application to RUN)
				if(DataFromFlash.APP_Number == BOOTLOADER_APP)
				{
					// Erase the flash memory part of Bank ONE before writing the application
					Flash_Status = Bootloader_Flash_Erase(APPLICATION_ONE);
					if(Flash_Status == SUCCESSFUL_ERASE)
					{
						// Write the application to Bank ONE
						Flash_Status = WRITE_TO_FLASH_FAILED;
						Flash_Status = Bootloader_WriteAppToFlash(App_In_SRAM, BOOTLOADER_ADDRESS_BANK_1, DataFromServer.file_size);
						// free the heap allocated space before jumping to execute the application
						//HAL_Delay(100);
						if(Flash_Status == WRITE_TO_FLASH_PASSED)
						{
							free(App_In_SRAM);
							// Erase the Non-Volatile part space of variables in flash before re
							Bootloader_Flash_Erase(NON_VOLATILE_PART);
							/* save the new information about the running application in the
							 * NON_VOLATILE_SPACE before jumping to execute */
							// save the Application number to APPLICATION_ONE and version
							Bootloader_Update_NONVOLATILE_SPACE(APPLICATION_ONE, DataFromServer.MAJOR_Version,
																    			 DataFromServer.MINOR_Version,
																				 DataFromServer.PATCH_Version);
							Bootloader_JumpToApp(BOOTLOADER_ADDRESS_BANK_1);
						}
					}

				}
				else if(DataFromFlash.APP_Number == APPLICATION_ONE)
				{
					// Erase the flash memory part of Bank TWO before writing the application
					Flash_Status = Bootloader_Flash_Erase(APPLICATION_TWO);
					if(Flash_Status == SUCCESSFUL_ERASE)
					{
						// Write the application to Bank TWO
						Flash_Status = WRITE_TO_FLASH_FAILED;
						Flash_Status = Bootloader_WriteAppToFlash(App_In_SRAM, BOOTLOADER_ADDRESS_BANK_2, DataFromServer.file_size);
						// free the heap allocated space before jumping to execute the application

						if(Flash_Status == WRITE_TO_FLASH_PASSED)
						{
							free(App_In_SRAM);
							// Erase the Non-Volatile part space of variables in flash before re
							Bootloader_Flash_Erase(NON_VOLATILE_PART);
							/* save the new information about the running application in the
							 * NON_VOLATILE_SPACE before jumping to execute */
							// save the Application number to APPLICATION_ONE and version
							Bootloader_Update_NONVOLATILE_SPACE(APPLICATION_TWO, DataFromServer.MAJOR_Version,
																				 DataFromServer.MINOR_Version,
																				 DataFromServer.PATCH_Version);
							Bootloader_JumpToApp(BOOTLOADER_ADDRESS_BANK_2);
						}
					}

				}
			}
		}

	}
	/* in case version of the server is the same of version stored in Flash
	 * jump to execute the application stored in flash */
	else if((DataFromServer.MAJOR_Version == DataFromFlash.MAJOR_Version)
			&& (DataFromServer.MINOR_Version == DataFromFlash.MINOR_Version)
			&& (DataFromServer.PATCH_Version == DataFromFlash.PATCH_Version))
	{
		// check which application to run from flash Application one
		if(DataFromFlash.APP_Number == APPLICATION_ONE)
		{
			//jump and execute application in BANK ONE in flash
			Bootloader_JumpToApp(BOOTLOADER_ADDRESS_BANK_1);
		}
		// check which application to run from flash Application one
		else if(DataFromFlash.APP_Number == APPLICATION_TWO)
		{
			//jump and execute application in BANK ONE in flash
			Bootloader_JumpToApp(BOOTLOADER_ADDRESS_BANK_2);
		}
		else{
			// No application in flash execute bootloader again
			asm("NOP");
		}
	}


	// in case of there is NO new version of application
}
/* Function to erase a specific part from flash memory */
FLASH_StatusTypedef Bootloader_Flash_Erase(uint8_t App_Number){

	FLASH_StatusTypedef 	Erase_Status = UNSUCCESSFUL_ERASE;
	FLASH_EraseInitTypeDef 	pEraseInit;
	HAL_StatusTypeDef 		HAL_Status = HAL_ERROR;
	uint32_t 				SectorError = 0;
	/* Erase the space of Application 1 (15k starting from 0x0800 5400 to 0x0800 9000) */
	if(App_Number == 1)
	{
		/* Initial FLASH sector to erase when Mass erase is disabled */
		pEraseInit.PageAddress = (uint32_t)(0x08005400UL);
		/* Number of pages to be erased.15 page (Application 1 size is 15k) */
		pEraseInit.NbPages = 15;
	}
	/* Erase the space of Application 2 (25k starting from 0x0800 5400 to 0x0800 9000) */
	else if(App_Number == 2)
	{
		/* Initial FLASH sector to erase when Mass erase is disabled */
		pEraseInit.PageAddress = (uint32_t)(0x08009000UL);
		/* Number of pages to be erased.15 page (Application 1 size is 15k) */
		pEraseInit.NbPages = 25;
	}
	/* Erase the space Non-Volatile Variables in flash  (1k starting from 0x0800 5000 to 0x0800 5400) */
	else if(App_Number == 3){
		/* Initial FLASH sector to erase when Mass erase is disabled */
		pEraseInit.PageAddress = (uint32_t)(0x08005000UL);
		/* Number of pages to be erased.15 page (Application 1 size is 15k) */
		pEraseInit.NbPages = 1;
	}
	/* Pages erase only */
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	/* Bank 1  */
	pEraseInit.Banks = FLASH_BANK_1;

	/* Unlock the FLASH control register access */
      HAL_Status = HAL_FLASH_Unlock();
			/* Perform a mass erase or erase the specified FLASH memory sectors */
			HAL_Status = HAL_FLASHEx_Erase(&pEraseInit, &SectorError);
			if(HAL_Status == HAL_OK && HAL_SUCCESSFUL_ERASE == SectorError){
				Erase_Status = SUCCESSFUL_ERASE;
			}
			else{
				Erase_Status = UNSUCCESSFUL_ERASE;
			}
			/* Locks the FLASH control register access */
      HAL_Status = HAL_FLASH_Lock();

	return Erase_Status;
}

/* Function to verify the validity of a specific address in flash memory or in SRAM */
FLASH_StatusTypedef 	Host_Address_Verification(uint32_t Address)
{
	FLASH_StatusTypedef Address_Verification = ADDRESS_IS_INVALID;
	if((Address >= SRAM_BASE) && (Address <= STM32F103_SRAM_END)){
		Address_Verification = ADDRESS_IS_VALID;
	}
	else if((Address >= FLASH_BASE) && (Address <= STM32F103_FLASH_END)){
		Address_Verification = ADDRESS_IS_VALID;
	}
	else{
		Address_Verification = ADDRESS_IS_INVALID;
	}
	return Address_Verification;
}

/* Function to Write Application to the flash memory */
FLASH_StatusTypedef 	Bootloader_WriteAppToFlash(uint8_t *App_Buffer, uint32_t App_Start_Address, uint16_t App_size)
{
	HAL_StatusTypeDef 	HAL_Status = HAL_ERROR;
	FLASH_StatusTypedef		Write_To_Flash_Status =	WRITE_TO_FLASH_FAILED;
	uint16_t			Index = 0;
	//uint16_t			Temp;
	uint16_t 			Data = 0;
	/*
	for(Index = 0; Index < App_size - (App_size % 2); Index+=2)
	{
		Temp = (uint16_t)App_Buffer[Index];
		App_Buffer[Index] = App_Buffer[Index+1];
		App_Buffer[Index+1] = (uint8_t)Temp;
	}
	for(Index = 0; Index < App_size - (App_size % 4); Index+=4)
	{
		Temp = *(uint16_t*)(App_Buffer+Index);
		*(uint16_t*)(App_Buffer+Index) = *(uint16_t*)(App_Buffer+Index+2);
		*(uint16_t*)(App_Buffer+Index+2)= Temp;
	}
*/

	// Unlock the FLASH control register access
  HAL_Status = HAL_FLASH_Unlock();

	if(HAL_Status != HAL_OK){
		Write_To_Flash_Status = WRITE_TO_FLASH_FAILED;
	}
	else{
			for(Index = 0; Index < App_size; Index+=2)
			{
				Data = *(uint16_t*)(App_Buffer+Index);
				HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (App_Start_Address + Index), Data);
				if(HAL_Status != HAL_OK){
					Write_To_Flash_Status = WRITE_TO_FLASH_FAILED;
					break;
				}
				else{
					Write_To_Flash_Status = WRITE_TO_FLASH_PASSED;
				}
			}

		}

	if((WRITE_TO_FLASH_PASSED == Write_To_Flash_Status) && (HAL_OK == HAL_Status)){
		// Locks the FLASH control register access
		HAL_Status = HAL_FLASH_Lock();
		if(HAL_Status != HAL_OK){
			Write_To_Flash_Status = WRITE_TO_FLASH_FAILED;
		}
		else{
			Write_To_Flash_Status = WRITE_TO_FLASH_PASSED;
		}
	}
	else{
		Write_To_Flash_Status = WRITE_TO_FLASH_FAILED;
	}

	return Write_To_Flash_Status;
}
/* Function that jump to execute the application */
void Bootloader_JumpToApp(uint32_t Address_To_Jump)
{
	FLASH_StatusTypedef 	Address_Verification = ADDRESS_IS_INVALID;

	// Verify the Extracted address to be valid address
	Address_Verification = Host_Address_Verification(Address_To_Jump);
	if(Address_Verification == ADDRESS_IS_VALID)
	{
		/* initialization bootloader */

		void (*App_Address) (void) = (void*)(*((volatile uint32_t *)(Address_To_Jump + 4U)));
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		Bootloader_DeInit();
		App_Address();
	}

}

/*	De-initialization of bootloader*/
void Bootloader_DeInit(void)
{
	// de-initialization of LED pins
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);
	// de-initialization UART pins (Tx, Rx)
	//HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
	//HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
	// de-initialization of UART
	HAL_UART_MspDeInit(HAL_UART_HANDLE);
	// de-initialization of RCC
	HAL_RCC_DeInit();
	HAL_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
}

/* Bootloader send command to ESPWIFI8266 */
ESP_StatusTypedef Bootloader_Send_Command(ESP_CMD Command)
{
	ESP_StatusTypedef ESP_Status = ESP_ERROR;
	ReceivingSize = 5;
	Void_UART_SettingUp();

	switch (Command)
	{
		// Wake-Up command
		case ESP_WAKE_UP 		: strcpy((char*)UART1_TxBuffer,"0");
		break;
		// GET_VERSION command
		case ESP_GET_VERSION 	: strcpy((char*)UART1_TxBuffer,"1");
		break;
		// GET_CRC command
		case ESP_GET_CRC 		: strcpy((char*)UART1_TxBuffer,"2");
		break;
		// GET_FILE_SIZE command
		case ESP_GET_FILE_SIZE 	: strcpy((char*)UART1_TxBuffer,"3");
		break;
		// GET_APP command
		case ESP_GET_APP 		: strcpy((char*)UART1_TxBuffer,"4");
		break;
		default: break;
	}
	strcat((char*)UART1_TxBuffer,"\n");

	while(ESP_Status != ESP_OK)
		{
			UART_Status = HAL_UART_Transmit(HAL_UART_HANDLE, (uint8_t*)UART1_TxBuffer, 2, HAL_MAX_DELAY);
			if(UART_Status == HAL_OK)
			{
				HAL_Delay(500);  //delay for ESP8266 to setup the response
				//while (timeout < 60000) {timeout++;}
				if(!strcmp((char*)UART1_RxBuffer,"ACK\r\n"))
				{ESP_Status = ESP_OK;}
				else {
					memset(UART1_RxBuffer,0,RxBuffer_size);
					HAL_UARTEx_ReceiveToIdle_IT(HAL_UART_HANDLE, (uint8_t*)UART1_RxBuffer, ReceivingSize);
				}

			}
		}
	return ESP_Status;
}
/*	 CRC32 Functions Algorithm */


// Generate CRC32 table
/*void generate_crc32_table() {
    const uint32_t polynomial = 0xEDB88320;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc & 1) ? (crc >> 1) ^ polynomial : (crc >> 1);
        }
        crc32_table[i] = crc;
    }
}*/

// Calculate CRC32 for given data
uint32_t calculate_crc32(const uint8_t *data, size_t length) {
	// CRC32 lookup table
	uint32_t crc32_table[256];
  	const uint32_t polynomial = 0xEDB88320;
    uint32_t crc = 0xFFFFFFFF;

 // Generate CRC32 table
	for (uint32_t i = 0; i < 256; i++) {
		uint32_t crc = i;
		for (int j = 0; j < 8; j++) {
			crc = (crc & 1) ? (crc >> 1) ^ polynomial : (crc >> 1);
		}
		crc32_table[i] = crc;
	}
	// Calculate CRC32 for given data
    for (size_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

void Bootloader_Receive_Server_Data(ESP_CMD Command)
{
	switch (Command)
	{
			// GET_VERSION command
			case ESP_GET_VERSION 	:
				ReceivingSize = 7;
				Void_UART_SettingUp();
				HAL_Delay(2000);
				//save the received data of latest update version
				DataFromServer.MAJOR_Version = (uint8_t)UART1_RxBuffer[0];
				DataFromServer.MINOR_Version = (uint8_t)UART1_RxBuffer[2];
				DataFromServer.PATCH_Version  = (uint8_t)UART1_RxBuffer[4];
			break;
			// GET_CRC command
			case ESP_GET_CRC 		:
				ReceivingSize = 12;
				Void_UART_SettingUp();
				HAL_Delay(2000);
				//save the received data of CRC
				DataFromServer.file_CRC = 0;
				for(uint8_t crc_indx = 0; crc_indx < CRC_NUMBER_OF_DIGITS; crc_indx++)
				{
					DataFromServer.file_CRC = DataFromServer.file_CRC * 10 + (UART1_RxBuffer[crc_indx] - '0');
				}

			break;
			// GET_FILE_SIZE command
			case ESP_GET_FILE_SIZE 	:
				ReceivingSize = 7;
				Void_UART_SettingUp();
				HAL_Delay(2000);
				//save the received data of file size
				DataFromServer.file_CRC = 0;
				for(uint8_t size_indx = 0; UART1_RxBuffer[size_indx] != '\r'; size_indx++)
				{
					DataFromServer.file_size = DataFromServer.file_size * 10 + (UART1_RxBuffer[size_indx] - '0');
				}

			break;
			// GET_APP command
			case ESP_GET_APP 		:
				ReceivingSize = PACKET_LENGTH;
				App_In_SRAM = (uint8_t*) calloc((DataFromServer.file_size) , sizeof(uint8_t));
				if(App_In_SRAM != NULL)
				{
					Void_UART_SettingUp();
					//HAL_Delay(1000);
				}

				//while(timeout < 100000) {timeout++;}
			break;
			default: break;
	}
}
