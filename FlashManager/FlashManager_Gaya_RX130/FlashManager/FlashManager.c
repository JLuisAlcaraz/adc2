/**
* @author David Martinez
*
* @brief The Flash Manager module is part of the Resource Manager block. The Flash Manager module
* gives a set of functions to allow -writing and- reading the data flash area on the microcontroller
* and handle it just like an E2PROM. -Orders for writing operations come from Memory Driver module
* and- the ones for reading operations comes from Memory Driver module, Variant Decoder module and
* Variant Table module.
* 
* @date 07/09/2018
* @version 0.0.1
*
* Copyright E.G.O. - All Rights Reserved
* The above copyright refers to the E.G.O. group development centers.
*/

#define __FLASHMANAGER_C__


/***************************************************************************************************
* Includes
***************************************************************************************************/

#include "../../../types.h"
#include "../../../Ssl/ResourceManager/SystemIntegrityTest/SystemIntegrity.h"
#include "../../../Ssl/ResourceManager/Interrupts/Interrupts.h"
#include "FlashManager.h"

//#include "..\..\..\Ssl\ResourceManager\IoSystem\IoSystemPins.h"



/***********************************************************************************************************************
Declarations of Private Functions
***********************************************************************************************************************/


static void FlashMan_WriteAByteDF(volatile u8 pucData, u32 ulAddr);
static void FlashMan_EnableAccessToDF(void);
//static void FlashMan_DisableAccessToDF(void);
static void FlashMan_ReadModeToPEmodeDF(void);
static void FlashMan_PEmodeToReadModeDF(void);

/***********************************************************************************************************************
 Functions
***********************************************************************************************************************/


/**
* @brief	This function initializes the hardware for current module. It is called by System
*			Initialization module. It configures the Data Flash memory for being read.
* @param	none
* @return	none
*/
void FlashManInit(void)
{		
	FlashMan_EnableAccessToDF();
}

/**
* @brief	This function reads the data on flash memory
* @param	pucData, the pointer gives data from flash memory
*			uiSize, number of bytes collect on pointer
*			ulAddr, address wherein you want to start reading
* @return	none
*/
void FlashMan_ReadDF(volatile u8 *pucData, u32 ulAddr, u16 uiSize)
{
	u16 i;
	
	//Pointer pointing to a flash memory address
	u8 *pucMemoryPos = (u8*)ulAddr;
	
	for(i=0;i<uiSize;i++)
	{
		pucData[i] = pucMemoryPos[i];
	}
}

/**
* @brief	This function gives access to the flash memory
* @param	none
* @return	none
*/
static void FlashMan_EnableAccessToDF(void)
{
	u16 i;
	
	FLASH.DFLCTL.BIT.DFLEN = 1;
	
	for(i=0;i<T_DSTOP;i++)
	{
		//Empty
		//Empty 2
		//Empty 3 
	}
}

/**
* @brief	This function removes access to the flash memory
* @param	none
* @return	none
*/
/*static void FlashMan_DisableAccessToDF(void)
{
	u8 i;	
	
	FLASH.DFLCTL.BIT.DFLEN = 0;
	for(i=0;i<T_DSTOP;i++);	
}*/

/**
* @brief	This function changes to program/erase mode for data flash
* @param	none
* @return	none
*/
static void FlashMan_ReadModeToPEmodeDF(void)
{
	FLASH.FENTRYR.WORD = 0xAA80;
	FLASH.FPR = 0xA5;
	FLASH.FPMCR.BYTE = 0x10;
	FLASH.FPMCR.BYTE = 0xEF;
	FLASH.FPMCR.BYTE = 0x10;
	FLASH.FISR.BIT.PCKA = 0x1F;
}

/**
* @brief	This function changes to read mode for data flash
* @param	none
* @return	none
*/
static void FlashMan_PEmodeToReadModeDF(void)
{
	u8 i;
	
	FLASH.FPR = 0xA5;
	FLASH.FPMCR.BYTE = 0x08;
	FLASH.FPMCR.BYTE = 0xF7;
	FLASH.FPMCR.BYTE = 0x08;
		
	
	for(i=0;i<T_MS;i++)
	{
		//Empty
	}
	
	FLASH.FENTRYR.WORD = 0xAA00;
	
	while(FLASH.FENTRYR.WORD != 0)
	{
		//Empty
	};
}

/**
* @brief	This function writes varius bytes in flash memory
* @param	pucData, pointer of bytes to write
*			uiSize, number of bytes to write
*			ulAddr, address wherein you want to start writing
* @return	none
*/
void FlashMan_WriteDF(const u8 *pucData, u32 ulAddr, u16 uiSize)
{
	u8 i;
	
	//Enter in program-erase mode
	FlashMan_ReadModeToPEmodeDF();

	for(i=0;i<uiSize;i++)
	{
		FlashMan_WriteAByteDF(*(pucData+i), ulAddr+i);
	}
	
	//Exit from program-erase mode
	FlashMan_PEmodeToReadModeDF();
}

/**
* @brief	This function writes a byte in flash memory
* @param	ucData, byte to write
*			ulAddr, address wherein you want to write it
* @return	none
*/
static void FlashMan_WriteAByteDF(volatile u8 ucData, u32 ulAddr)
{	
	FLASH.FASR.BIT.EXS = 0;
	
	FLASH.FSARH = (u16)(ulAddr >> 16);
	FLASH.FSARL = (u16)(ulAddr & 0x0000FFFF);
	//in flash memory only it is possible writing byte to byte
	FLASH.FWBH = 0;
	FLASH.FWBL = (u16)ucData;
	FLASH.FCR.BYTE = 0x81;
	
	while(FLASH.FSTATR1.BIT.FRDY == 0)
	{
		//Empty
	};
	
	FLASH.FCR.BYTE = 0;
	
	while(FLASH.FSTATR1.BIT.FRDY != 0)
	{
		//Empty
	};
	
	if((FLASH.FSTATR0.BIT.ILGLERR != 0) ||
		(FLASH.FSTATR0.BIT.PRGERR != 0))
	{
		FLASH.FRESETR.BYTE = 1;
		FLASH.FRESETR.BYTE = 0;
	}
	else
	{
		//Empty
	}
}

/**
* @brief	This function erases a Flash Memory Block
* @param	ulAddr, Block address which is wanted to erase
* @return	none
*/
void FlashMan_BlockEraseDF(u32 ulAddr)
{
	u32 ulAddr_end = ulAddr + (FLASHMAN_BLOCK_SIZE - 1);
	
	//Enter in program-erase mode
	FlashMan_ReadModeToPEmodeDF();

	FLASH.FASR.BIT.EXS = 0;
	
	FLASH.FSARH = (u16)(ulAddr >> 16);
	FLASH.FSARL = (u16)(ulAddr & 0x0000FFFF);
	
	FLASH.FEARH = (u16)(ulAddr_end >> 16);
	FLASH.FEARL = (u16)(ulAddr_end & 0x0000FFFF);
	
	FLASH.FCR.BYTE = 0x84;
	
	while(FLASH.FSTATR1.BIT.FRDY == 0)
	{
		//Empty
	};


	FLASH.FCR.BYTE = 0;
	
	while(FLASH.FSTATR1.BIT.FRDY != 0)
	{
		//Empty
	};
	
	if((FLASH.FSTATR0.BIT.ILGLERR != 0) ||
		(FLASH.FSTATR0.BIT.ERERR != 0))
	{
		FLASH.FRESETR.BYTE = 1;
		FLASH.FRESETR.BYTE = 0;
	}
	else
	{
		//Empty
	}
	
	//Exit from program-erase mode
	FlashMan_PEmodeToReadModeDF();	
}

