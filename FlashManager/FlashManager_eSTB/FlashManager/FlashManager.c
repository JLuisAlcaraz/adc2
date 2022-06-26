/**
* @brief The Flash Manager module is part of the Resource Manager block.
* The Flash Manager module gives a set of functions to allow writing and reading the data flash area, on the microcontroller.
* and handle it just like an E2PROM.
* Orders for writing operations come from Memory Driver module * and- the ones for reading operations comes from Memory
* Driver module,
*
* Copyright E.G.O. - All Rights Reserved
* The above copyright refers to the E.G.O. group development centers.
*/

/***********************************************************************************************************************
* Includes
***********************************************************************************************************************/

/* Renesas Generated code includes  */
#include "FlashManager.h"
#include "r_cg_macrodriver.h"

#include "../../../types.h"
//#include "../../../Ssl/ResourceManager/SystemIntegrityTest/SystemIntegrity.h"
//#include "../../../Ssl/ResourceManager/Interrupts/Interrupts.h"
//#include "../../../Ssl/ResourceManager/Scheduling/Scheduling.h"

/***********************************************************************************************************************
* Defines
***********************************************************************************************************************/
#define T_DSTOP     	30    //Min 250ns (RX140 Hardware manual - Electrical Characteristics)
#define T_TMS			120   //Min 2us
#define T_MS	        30
#define FPR_KEY			0xA5  //In order to write on FPMCR, first must write this value to FPR.
#define E2FLASH_PEMODE  0x10
#define E2FLASH_READMODE 0x08

//info about all area
#define FLASHMAN_BLOCK_SIZE	0x400
#define FLASHMAN_BLOCK_NUM	8

//addresses of each block (read mode)
#define FLASHMAN_BLOCK_ADDR_RD	0x00100000
#define	FLASHMAN_BLOCK_0_RD		( FLASHMAN_BLOCK_ADDR_RD )
#define	FLASHMAN_BLOCK_1_RD		( FLASHMAN_BLOCK_0_RD + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_2_RD		( FLASHMAN_BLOCK_1_RD + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_3_RD		( FLASHMAN_BLOCK_2_RD + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_4_RD		( FLASHMAN_BLOCK_3_RD + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_5_RD		( FLASHMAN_BLOCK_4_RD + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_6_RD		( FLASHMAN_BLOCK_5_RD + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_7_RD		( FLASHMAN_BLOCK_6_RD + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_7_RD_END	( FLASHMAN_BLOCK_7_RD + FLASHMAN_BLOCK_SIZE - 1)

//addresses of each block (write mode)
#define FLASHMAN_BLOCK_ADDR_WR	0xFE000000
#define	FLASHMAN_BLOCK_0_WR		( FLASHMAN_BLOCK_ADDR_WR )
#define	FLASHMAN_BLOCK_1_WR		( FLASHMAN_BLOCK_0_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_2_WR		( FLASHMAN_BLOCK_1_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_3_WR		( FLASHMAN_BLOCK_2_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_4_WR		( FLASHMAN_BLOCK_3_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_5_WR		( FLASHMAN_BLOCK_4_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_6_WR		( FLASHMAN_BLOCK_5_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_7_WR		( FLASHMAN_BLOCK_6_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_7_WR_END	( FLASHMAN_BLOCK_7_WR + FLASHMAN_BLOCK_SIZE - 1)

#define FLASHMAN_STATUS_OK		(0x00U)
#define FLASHMAN_STATUS_ERROR	(0x01U)
#define FLASHMAN_ERASE_STATUS	(FLASH.FSTATR0.BIT.ERERR)
#define FLASHMAN_WRITE_STATUS 	(FLASH.FSTATR0.BIT.PRGERR)


/***********************************************************************************************************************
* Declarations of Private Functions
***********************************************************************************************************************/
void FlashManInit(void);
void FlashManDeInit(void);
static uint8_t FlashMan_WriteAByteDF(volatile uint8_t pucData, uint32_t ulAddr);
static uint8_t FlashMan_ReadModeToPEmodeDF(void);
static void FlashMan_PEmodeToReadModeDF(void);

/***********************************************************************************************************************
*  Functions
***********************************************************************************************************************/

/*static uint8_t E2FlashCheckHOCO(void)
{
	if(SYSTEM.OSCOVFSR.BIT.HCOVF)
	{
		return 0;
	}
	else
	{
		return ERR_FLASHE2DATA_NOHOCO;
	}

}
*/



/**
* @brief	This function changes to program/erase mode for data flash
* @param	none
* @return	none
*/
static uint8_t FlashMan_ReadModeToPEmodeDF(void)
{
	//Debería devolver el estado de FPSR.ERR, indicando si ha habido error al modificar FPMCR??

	uint16_t i;

	//Check for HOCO running and stabilized
	if(!SYSTEM.OSCOVFSR.BIT.HCOVF)
	{
		return ERR_FLASHE2DATA_NOHOCO;
	}

	//Check no Low-Speed operating mode selected
	if(SYSTEM.SOPCCR.BIT.SOPCM)
	{
		return ERR_FLASHE2DATA_LOWSPEED;
	}

	//E2FLASH a modo P/E
	FLASH.FENTRYR.WORD = 0xAA80;  //FEKEY = 0xAA00 + FENTRYD = 0x0080
	for(i=0;i<T_DSTOP;i++){}
	if(!FLASH.FENTRYR.BIT.FENTRYD)
	{
		return ERR_FLASHE2DATA_NOPEMODE;
	}

	FLASH.FPR = FPR_KEY;
	//Transition from ReadMode to E2 P/E Mode
	FLASH.FPMCR.BYTE = E2FLASH_PEMODE;
	FLASH.FPMCR.BYTE = ~(E2FLASH_PEMODE);
	FLASH.FPMCR.BYTE = E2FLASH_PEMODE;

	//TO DO
	//Add Check FCLOCK for 48MHZ!!!!!!
	FLASH.FISR.BIT.PCKA = 0x27;   //0x27 is for a FCLOCK of 48MHz  //0x1F;

	return 0;
}


/**
* @brief	This function changes to read mode for data flash
* @param	none
* @return	none
*/
static void FlashMan_PEmodeToReadModeDF(void)
{
	uint8_t i;
	
	
	//E2FLASH a modo READ
	FLASH.FPR = FPR_KEY;
	FLASH.FPMCR.BYTE = E2FLASH_READMODE;
	FLASH.FPMCR.BYTE = ~(E2FLASH_READMODE);
	FLASH.FPMCR.BYTE = E2FLASH_READMODE;
	for(i=0;i<T_TMS;i++){}

	FLASH.FENTRYR.WORD = 0xAA00;
	while(FLASH.FENTRYR.WORD != 0) {}
}


/**
* @brief	This function writes a byte in flash memory
* @param	ucData, byte to write
* @param	ulAddr, address wherein you want to write it
* @return	Write/program status
*/
static uint8_t FlashMan_WriteAByteDF(volatile uint8_t ucData, uint32_t ulAddr)
{
	//mHwIWatchdogRefresh();  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	FLASH.FASR.BIT.EXS = 0;
	
	FLASH.FSARH = (uint16_t)(ulAddr >> 16);
	FLASH.FSARL = (uint16_t)(ulAddr & 0x0000FFFF);
	
	FLASH.FWB0 = ucData;

	FLASH.FCR.BYTE = 0x81;
	
	while(FLASH.FSTATR1.BIT.FRDY == 0)
	{
	}
	
	FLASH.FCR.BYTE = 0;
	
	while(FLASH.FSTATR1.BIT.FRDY != 0)
	{
	}
	
	if((FLASH.FSTATR0.BIT.ILGLERR) || (FLASH.FSTATR0.BIT.PRGERR))
	{
		FLASH.FRESETR.BYTE = 1;
		FLASH.FRESETR.BYTE = 0;
	}
	else
	{
		// Empty
	}
	
	return (uint8_t)FLASHMAN_WRITE_STATUS;
}




/**
* @brief	This function initializes the hardware for current module. It is called by System
*			Initialization module. This function gives access to the flash memory
* @param	none
* @return	none
*/
void FlashManInit(void)
{
	uint16_t i;

	FLASH.DFLCTL.BIT.DFLEN = 1;
	for(i=0;i<T_DSTOP;i++){}
}

void FlashManDeInit(void)
{
	uint16_t i;

	FLASH.DFLCTL.BIT.DFLEN = 0;
	for(i=0;i<T_DSTOP;i++){}
}

/**
* @brief	This function reads the data on flash memory
* @param	pucData, the pointer gives data from flash memory
*			uiSize, number of bytes collect on pointer
*			ulAddr, address wherein you want to start reading
* @return	none
*/
uint8_t FlashMan_ReadDF(volatile uint8_t *pucData, uint32_t ulAddr, uint16_t uiSize)
{
	uint16_t i;

	//Habilitar acceso a E2FLASH
	FlashManInit();

	//E2FLASH a modo READ
	FlashMan_PEmodeToReadModeDF();

	//La dirección ulAddr está dentro del rango de la E2FLASH??
	//Se toma un bloque de 8k para la E2FLASH
	if(ulAddr < E2FLASHADDR_READBASE || ulAddr > (E2FLASHADDR_READBASE + E2FLASHBLOCK_15))
	{
		return ERR_FLASHE2DATA_OUTRNG;
	}


	//Pointer pointing to a flash memory address
	uint8_t *pucMemoryPos = (uint8_t*)ulAddr;

	for(i=0;i<uiSize;i++)
	{
		pucData[i] = pucMemoryPos[i];
	}

	//Deshabilitar acceso a E2FLASH
	FlashManInit();

	return 0;
}


/**
* @brief	This function writes varius bytes in flash memory
* @param	pucData, pointer of bytes to write
* @param	uiSize, number of bytes to write
* @param	ulAddr, address wherein you want to start writing
* @return	Write status
*/
uint8_t FlashMan_WriteDF(const uint8_t *pucData, uint32_t ulAddr, uint16_t uiSize)
{
	uint16_t i;
	uint8_t ucReturn=FLASHMAN_STATUS_ERROR;

	//Habilitar acceso a E2FLASH
	FlashManInit();

	//E2Flash a mode P/E
	FlashMan_ReadModeToPEmodeDF();


	for(i=0;i<uiSize;i++)
	{
		ucReturn = FlashMan_WriteAByteDF(*(pucData+i), ulAddr+i);

		if (ucReturn != FLASHMAN_STATUS_OK)	{ break; 		}	//Exit the for loop
		else								{ /* EMPTY */	}
	}

	//Deshabilita acceso a E2FLASH
	FlashManDeInit();

	return ucReturn;
}


/**
* @brief	This function erases a Flash Memory Block
* @param	ulAddr, Block address which is wanted to erase
* @return	none
*/
uint8_t FlashMan_BlockEraseDF(uint32_t ulAddr)
{	
	uint8_t i;
	uint8_t ucSkip=0;
	//Enter in program-erase mode
	FlashMan_ReadModeToPEmodeDF();

	FLASH.FASR.BIT.EXS = 0;

	//mHwIWatchdogRefresh();  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	for(i = 0; i < FLASHMAN_BLOCK_NUM; i++)
	{
		if(	(ulAddr >= (FLASHMAN_BLOCK_ADDR_WR + (i*FLASHMAN_BLOCK_SIZE)))
			&&(ulAddr < (FLASHMAN_BLOCK_ADDR_WR + ((i+1)*FLASHMAN_BLOCK_SIZE)))&&(ucSkip==0))
		{
			FLASH.FSARH = (uint16_t)((FLASHMAN_BLOCK_ADDR_WR + (i*FLASHMAN_BLOCK_SIZE)) >> 16);
			FLASH.FSARL = (uint16_t)((FLASHMAN_BLOCK_ADDR_WR + (i*FLASHMAN_BLOCK_SIZE)) & 0x0000FFFF);

			FLASH.FEARH = (uint16_t)((FLASHMAN_BLOCK_ADDR_WR + ((i+1)*FLASHMAN_BLOCK_SIZE)) >> 16);
			FLASH.FEARL = (uint16_t)((FLASHMAN_BLOCK_ADDR_WR + (((i+1)*FLASHMAN_BLOCK_SIZE) - 1)) & 0x0000FFFF);

			ucSkip=1; // to avoid using a break sentence
		}
	}

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
	
	return (uint8_t)FLASHMAN_ERASE_STATUS;
}

