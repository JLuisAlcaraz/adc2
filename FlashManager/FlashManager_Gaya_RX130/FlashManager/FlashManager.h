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

#ifndef __FLASHMANAGER_H__
#define __FLASHMANAGER_H__


/***************************************************************************************************
* Includes
***************************************************************************************************/

#include "../../../types.h"


/***************************************************************************************************
* Defines
***************************************************************************************************/
#define T_DSTOP  30
#define T_MS  30


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

//addresses of each block (write mode)
#define FLASHMAN_BLOCK_ADDR_WR	0xF1000
#define	FLASHMAN_BLOCK_0_WR		( FLASHMAN_BLOCK_ADDR_WR )
#define	FLASHMAN_BLOCK_1_WR		( FLASHMAN_BLOCK_0_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_2_WR		( FLASHMAN_BLOCK_1_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_3_WR		( FLASHMAN_BLOCK_2_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_4_WR		( FLASHMAN_BLOCK_3_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_5_WR		( FLASHMAN_BLOCK_4_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_6_WR		( FLASHMAN_BLOCK_5_WR + FLASHMAN_BLOCK_SIZE )
#define	FLASHMAN_BLOCK_7_WR		( FLASHMAN_BLOCK_6_WR + FLASHMAN_BLOCK_SIZE )







/***************************************************************************************************
* Declarations of Public Functions
***************************************************************************************************/

void FlashManInit(void);
void FlashMan_ReadDF(volatile u8 *pucData, u32 ulAddr, u16 uiSize);
void FlashMan_WriteDF(const u8 *pucData, u32 ulAddr, u16 uiSize);
void FlashMan_BlockEraseDF(u32 ulAddr);



#endif // __FLASHMANAGER_H__
