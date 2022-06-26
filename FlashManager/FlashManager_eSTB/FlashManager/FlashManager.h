/**
* @brief The Flash Manager module is part of the Resource Manager block. The Flash Manager module gives a set of 
* functions to allow -writing and- reading the data flash area on the microcontrollerand handle it just like an E2PROM.
* -Orders for writing operations come from Memory Driver module * and- the ones for reading operations comes from Memory
* Driver module,
*
* Copyright E.G.O. - All Rights Reserved
* The above copyright refers to the E.G.O. group development centers.
*/
#ifndef __FLASHMANAGER_H__
#define __FLASHMANAGER_H__
/***********************************************************************************************************************
* Includes
***********************************************************************************************************************/
#include "../../../types.h"


/***********************************************************************************************************************
* Defines
***********************************************************************************************************************/
#define ERR_FLASHE2DATA_NOREADMODE	1
#define ERR_FLASHE2DATA_OUTRNG      2
#define ERR_FLASHE2DATA_NOHOCO      3
#define ERR_FLASHE2DATA_NOPEMODE	4
#define ERR_FLASHE2DATA_LOWSPEED	5


#define E2FLASHADDR_READBASE	0x00100000
#define E2FLASHADDR_WRITEBASE	0xFE000000
#define E2FLASHBLOCK_00			0x0000
#define E2FLASHBLOCK_01			0x0100
#define E2FLASHBLOCK_02			0x0200
#define E2FLASHBLOCK_03			0x0300
#define E2FLASHBLOCK_04			0x0400
#define E2FLASHBLOCK_05			0x0500
#define E2FLASHBLOCK_06			0x0600
#define E2FLASHBLOCK_07			0x0700
#define E2FLASHBLOCK_08			0x0800
#define E2FLASHBLOCK_09			0x0900
#define E2FLASHBLOCK_10			0x0A00
#define E2FLASHBLOCK_11			0x0B00
#define E2FLASHBLOCK_12			0x0C00
#define E2FLASHBLOCK_13			0x0D00
#define E2FLASHBLOCK_14			0x0E00
#define E2FLASHBLOCK_15			0x0F00


/***********************************************************************************************************************
* Declarations of Public Functions
***********************************************************************************************************************/
void FlashManInit(void);
static uint8_t E2FlashCheckHOCO(void);
u8 FlashMan_ReadDF(volatile uint8_t *pucData, uint32_t ulAddr, uint16_t uiSize);
uint8_t FlashMan_WriteDF(const uint8_t *pucData, uint32_t ulAddr, uint16_t uiSize);
uint8_t FlashMan_BlockEraseDF(uint32_t ulAddr);


#endif // __FLASHMANAGER_H__
