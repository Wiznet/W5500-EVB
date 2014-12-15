
#include "w5500_spi.h"
#include "w5500evb_dataflash.h"


// Dataflash opcodes
/*
#define FlashPageRead				0x52	// Main memory page read
#define FlashToBuf1Transfer 		0x53	// Main memory page to buffer 1 transfer
#define Buf1Read					0x54	// Buffer 1 read
#define FlashToBuf2Transfer 		0x55	// Main memory page to buffer 2 transfer
#define Buf2Read					0x56	// Buffer 2 read
//#define StatusReg					0x57	// Status register
#define StatusReg					0xD7	// Status register
#define AutoPageReWrBuf1			0x58	// Auto page rewrite through buffer 1
#define AutoPageReWrBuf2			0x59	// Auto page rewrite through buffer 2
#define FlashToBuf1Compare    		0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	    	0x61	// Main memory page to buffer 2 compare
#define ContArrayRead				0x68	// Continuous Array Read (Note : Only A/B-parts supported)
#define FlashProgBuf1				0x82	// Main memory page program through buffer 1
#define Buf1ToFlashWE   			0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf1Write					0x84	// Buffer 1 write
#define FlashProgBuf2				0x85	// Main memory page program through buffer 2
#define Buf2ToFlashWE   			0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf2Write					0x87	// Buffer 2 write
#define Buf1ToFlash     			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash		         	0x89	// Buffer 2 to main memory page program without built-in erase
*/

void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr);
unsigned char df_read_status (void);

static void DF_CS_INIT(void);
static void DF_CS_LOW(void);
static void DF_CS_HIGH(void);
static void  DF_SPI_RW(uint8_t wb);
static uint8_t DF_SPI_RD(void);


void DF_CS_INIT(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 19, (IOCON_FUNC0 | IOCON_MODE_PULLUP));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1,  19);	// On-board Dataflash SSEL(CS) as GPIO output
}

void  DF_CS_LOW(void)
{
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 19, false);	// SSEL(CS)
}

void  DF_CS_HIGH(void)
{
	Chip_GPIO_SetPinState(LPC_GPIO, 1, 19, true);	// SSEL(CS)
}

void  DF_SPI_RW(uint8_t wb)
{
	Chip_SSP_WriteFrames_Blocking(LPC_SSP0, &wb, 1);
}

uint8_t DF_SPI_RD(void)
{
	uint8_t rb;
	Chip_SSP_ReadFrames_Blocking(LPC_SSP0, &rb, 1);
	return rb;
}

void DF_Init(void)
{
	DF_CS_INIT();	//Data Flah CS Initialize
	DF_CS_HIGH();
		
	DF_SPI_INIT();
	
	//df_read_status();
}

void DF_Chip_Erase(void)
{
	DF_CS_LOW();
	__disable_irq();	// Global interrupt disable (ARM)

	DF_SPI_RW(0xc7);	// cmd
	DF_SPI_RW(0x94);	// cmd
	DF_SPI_RW(0x80);	// cmd
	DF_SPI_RW(0x9a);	// cmd

	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high

	__enable_irq();		// Global interrupt enable (ARM)
	DF_CS_HIGH();
}

unsigned char DF_BufferReadByte(unsigned char BufferNo, unsigned int IntPageAdr)
{
	unsigned char data = 0;
	
	DF_CS_LOW();
	__disable_irq();	// Global interrupt disable (ARM)

	if(1 == BufferNo)							//read byte from buffer 1
	{
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00); ##						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00); ##  					//don't cares
		//data = DF_SPI_RW(0x00);					//read byte
		data = DF_SPI_RD();
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//read byte from buffer 2
	{
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00);						//don't cares
		data = DF_SPI_RW(0x00);					//read byte
	}
#endif

	__enable_irq();		// Global interrupt enable (ARM)
	DF_CS_HIGH();		
	return data;								//return the read data byte
}


void DF_BufferWriteByte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
	DF_CS_LOW();
	__disable_irq();	// Global interrupt disable (ARM)
	
	if (1 == BufferNo)							//write byte to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00);	##					//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(Data);						//write data byte
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//write byte to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(Data);						//write data byte
	}		
#endif

	__enable_irq();		// Global interrupt enable (ARM)
	DF_CS_HIGH();	
}


// Globals
unsigned char PageBits;
unsigned int  PageSize;
// Functions

/*****************************************************************************
*
*	Function name : df_read_status
*
*	Returns :		One status byte. Consult Dataflash datasheet for further
*					decoding info
*
*	Parameters :	None
*
*	Purpose :		Status info concerning the Dataflash is busy or not.
*					Status info concerning compare between buffer and flash page
*					Status info concerning size of actual device
*
******************************************************************************/
unsigned char df_read_status (void)
{
	unsigned char result,index_copy;
	//Look-up table for these sizes ->  512k, 1M,  2M,  4M,  8M, 16M, 32M, 64M
	unsigned int DF_pagesize[] ={264, 264, 264, 264, 264, 528, 528, 1056};
	//Look-up table for these sizes ->  512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
	unsigned char DF_pagebits[] ={  9,  9,  9,  9,  9,  10,  10,  11};
	
	
							//make sure to toggle CS signal in order
	DF_CS_LOW();							//to reset dataflash command decoder
	//__disable_irq();	// Global interrupt disable (ARM)

	//result = DF_SPI_RW(StatusReg);			//send status register read op-code
	//result = DF_SPI_RW(0x00);				//dummy write to get result
	DF_SPI_RW(StatusReg);
	result = DF_SPI_RD();

	index_copy = ((result & 0x38) >> 3);	//get the size info from status register

	PageBits   = DF_pagebits[index_copy];	//get number of internal page address bits from look-up table
	PageSize   = DF_pagesize[index_copy];   //get the size of the page (in bytes)

	//__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();
	return result;							//return the read status register value
}


/*****************************************************************************
*
*	Function name : DF_PageToBuffer
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of page to be transferred to buffer
*
*	Purpose :		Transfers a page from flash to dataflash SRAM buffer
*					
******************************************************************************/
void DF_PageToBuffer(unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_LOW();												//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)
	
	if (1 == BufferNo)											//transfer flash page to buffer 1
	{
		DF_SPI_RW(FlashToBuf1Transfer);							//transfer to buffer 1 op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);  										//don't cares
		//DF_SPI_RW(0x00); ##						//don't cares
	}
#ifdef USE_BUFFER2
	else	
	if (2 == BufferNo)											//transfer flash page to buffer 2
	{
		DF_SPI_RW(FlashToBuf2Transfer);							//transfer to buffer 2 op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
#endif
	
	DF_CS_HIGH();												//initiate the transfer
	DF_CS_LOW();

	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high
	
	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();		
}

void DF_BufferToPage (unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_LOW();												//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)

	if (1 == BufferNo)											//program flash page from buffer 1
	{
		DF_SPI_RW(Buf1ToFlashWE);								//buffer 1 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00);	##									//don't cares
	}

#ifdef USE_BUFFER2
	else	
	if (2 == BufferNo)											//program flash page from buffer 2
	{
		DF_SPI_RW(Buf2ToFlashWE);								//buffer 2 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
#endif
	
	DF_CS_HIGH();												//initiate flash page programming
	DF_CS_LOW();												
	
	while(!(df_read_status() & 0x80));							//monitor the status register, wait until busy-flag is high

	__enable_irq();	// Global interrupt disable (ARM)
	DF_CS_HIGH();	
}


/*****************************************************************************
*
*	Function name : DF_BufferReadStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be read
*					*BufferPtr	->	address of buffer to be used for read bytes
*
*	Purpose :		Reads one or more bytes from one of the dataflash
*					internal SRAM buffers, and puts read bytes into
*					buffer pointed to by *BufferPtr
*
******************************************************************************/
void DF_BufferReadStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_LOW();								//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)
	
	if (1 == BufferNo)							//read byte(s) from buffer 1
	{
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00);	##					//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00);	##					//don't cares
		for( i=0; i<No_of_bytes; i++)
		{
			//*(BufferPtr) = DF_SPI_RW(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
			*(BufferPtr) = DF_SPI_RD();			//read byte and put it in AVR buffer pointed to by *BufferPtr
			BufferPtr++;						//point to next element in AVR buffer
		}
	}
	
#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//read byte(s) from buffer 2
	{
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00);						//don't cares
		for( i=0; i<No_of_bytes; i++)
		{
			//*(BufferPtr) = DF_SPI_RW(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
			*(BufferPtr) = DF_SPI_RD();			//read byte and put it in AVR buffer pointed to by *BufferPtr
			BufferPtr++;						//point to next element in AVR buffer
		}
	}
#endif

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();	
}
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..


/*****************************************************************************
*
*	Function name : DF_BufferWriteEnable
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to start writing from
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					
*	Purpose :		Enables continous write functionality to one of the dataflash buffers
*					buffers. NOTE : User must ensure that CS goes high to terminate
*					this mode before accessing other dataflash functionalities 
*
******************************************************************************/
void DF_BufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr)
{
	DF_CS_LOW();								//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)
	
	if (1 == BufferNo)							//write enable to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00);	##					//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	}
	
#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//write enable to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	}
#endif

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();	
}


/*****************************************************************************
*
*	Function name : DF_BufferWriteStr
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be written
*					*BufferPtr	->	address of buffer to be used for copy of bytes
*									from AVR buffer to dataflash buffer 1 (or 2)
*
*	Purpose :		Copies one or more bytes to one of the dataflash
*					internal SRAM buffers from AVR SRAM buffer
*					pointed to by *BufferPtr
*
******************************************************************************/
void DF_BufferWriteStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_LOW();								//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)
	
	if (1 == BufferNo)							//write byte(s) to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00); 						//don't cares
		//DF_SPI_RW(0x00);	##					//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		for( i=0; i<No_of_bytes; i++)
		{
			DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 1 location
			BufferPtr++;						//point to next element in AVR buffer
		}
	}

#ifdef USE_BUFFER2
	else
	if (2 == BufferNo)							//write byte(s) to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		for( i=0; i<No_of_bytes; i++)
		{
			DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 2 location
			BufferPtr++;						//point to next element in AVR buffer
		}
	}
#endif

	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();	
}
//NB : Monitorer busy-flag i status-reg.
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..






/*****************************************************************************
*
*	Function name : Cont_Flash_Read_Enable
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
*					IntPageAdr	->	Internal page address where cont.read starts from
*
*	Purpose :		Initiates a continuous read from a location in the DataFlash
*					
******************************************************************************/
void Cont_Flash_Read_Enable (unsigned int PageAdr, unsigned int IntPageAdr)
{
	DF_CS_LOW();																//to reset dataflash command decoder
	__disable_irq();	// Global interrupt disable (ARM)
	
	DF_SPI_RW(ContArrayRead);													//Continuous Array Read op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));						//upper part of page address
	DF_SPI_RW((unsigned char)((PageAdr << (PageBits - 8))+ (IntPageAdr>>8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW((unsigned char)(IntPageAdr));										//LSB byte of internal page address
	DF_SPI_RW(0x00);															//perform 4 dummy writes
	DF_SPI_RW(0x00);															//in order to intiate DataFlash
	DF_SPI_RW(0x00);															//address pointers
	DF_SPI_RW(0x00);
	
	__enable_irq();	// Global interrupt enable (ARM)
	DF_CS_HIGH();		
}

