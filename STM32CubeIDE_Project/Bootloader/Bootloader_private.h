/*
 * Bootloader_priv.h
 *
 *  Created on: Oct 29, 2023
 *      Author: joe
 */

#ifndef BOOTLOADER_PRIVATE_H_
#define BOOTLOADER_PRIVATE_H_

typedef struct {
	volatile uint16_t	 	APP_TO_RUN;
	volatile uint16_t   	LATEST_VERSION[3];
	volatile uint16_t 		initialized;
}NON_VOLATILE_SPACE;

#define NON_VOLATILE_VARIABLE	(* ((volatile NON_VOLATILE_SPACE* ) BOOTLOADER_ADDRESS_VARIABLES))

// Data from flash to be accessed faster
struct FASLH {

	uint8_t 	MAJOR_Version;
	uint8_t 	MINOR_Version;
	uint8_t 	PATCH_Version;
	uint16_t 	APP_Number;
};

extern struct FASLH DataFromFlash;

/* Bootloader status */

typedef enum{
	BL_ERROR,
	BL_OK
}BL_STATUS;

/* CRC status*/
typedef enum{
	CRC_VERIFICATION_FAILED,
	CRC_VERIFICATION_PASSED
}CRC_STATUS;

/* Erase status*/

typedef enum{
	UNSUCCESSFUL_ERASE,
	SUCCESSFUL_ERASE,
	ADDRESS_IS_INVALID,
	ADDRESS_IS_VALID,
	WRITE_TO_FLASH_FAILED,
	WRITE_TO_FLASH_PASSED
}FLASH_StatusTypedef;


/* ESP Commands */
typedef enum{
	ESP_WAKE_UP,
	ESP_GET_VERSION,
	ESP_GET_CRC,
	ESP_GET_FILE_SIZE,
	ESP_GET_APP
}ESP_CMD;

/*	ESP Status */
typedef enum{
	ESP_ERROR,
	ESP_OK

}ESP_StatusTypedef;


/*		static functions declarations		*/
void 			Void_UART_SettingUp (void);
FLASH_StatusTypedef 	Bootloader_WriteAppToFlash(uint8_t *App_Buffer, uint32_t App_Start_Address, uint16_t App_size);
FLASH_StatusTypedef 	Bootloader_Flash_Erase(uint8_t App_Number);
void 	Bootloader_JumpToApp(uint32_t Address_To_Jump);
void Bootloader_DeInit(void);
ESP_StatusTypedef Bootloader_Send_Command(ESP_CMD Command);
uint32_t calculate_crc32(const uint8_t *data, size_t length);
void Bootloader_Receive_Server_Data(ESP_CMD Command);
void Bootloader_Update_NONVOLATILE_SPACE(uint16_t App_Number, uint16_t MAJOR, uint16_t MINOR, uint16_t PATCH);

/**
  * @brief  Calculate the CRC for the received data based on HAL_CRC_Accumulate function
  *         parameters in the CRC_InitTypeDef and create the associated handle.
  * @param  pData pointer to the data whose CRC calculated for
  * @param  Data_Len length of the data (number of elements)
  * @param  Host_CRC the received CRC from the sender
  * @retval
  */
/*CRC_STATUS	 	Bootloader_CRC_Verify(uint8_t *pData, uint32_t Data_Len, uint32_t Host_CRC);

FLASH_STATUS 	Host_Address_Verification(uint32_t Jump_Address);
//FLASH_STATUS 	Bootloader_WriteAppToFlash(uint8_t *App_Buffer);



static uint8_t 	Flash_Memory_Write_Payload(uint8_t *Host_Payload, uint32_t Payload_Start_Address, uint16_t Payload_Len);
*/



#endif /* BOOTLOADER_PRIVATE_H_ */
