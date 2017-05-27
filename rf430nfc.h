#ifndef RF430NFC_H_
#define RF430NFC_H_

#include "msp430.h"
#include "stdint.h"

void RF430_I2C_Init(void);
void Write_Register(unsigned int reg_addr, unsigned int value);
unsigned int Read_Register(unsigned int reg_addr);
void Write_Continuous(unsigned int reg_addr, unsigned char* write_data, unsigned int data_length);
void RF430_Init(void);

#define RF430_I2C_ADDR 	0x0018

//define the values for Granite's registers we want to access
#define CONTROL_REG 			0xFFFE
#define STATUS_REG				0xFFFC
#define INT_ENABLE_REG			0xFFFA
#define INT_FLAG_REG			0xFFF8
#define CRC_RESULT_REG			0xFFF6
#define CRC_LENGTH_REG			0xFFF4
#define CRC_START_ADDR_REG		0xFFF2
#define COMM_WD_CTRL_REG		0xFFF0
#define VERSION_REG				0xFFEE //contains the software version of the ROM
#define NDEF_FILE_ID			0xFFEC
#define HOST_RESPONSE			0xFFEA
#define NDEF_FILE_LENGTH		0xFFE8
#define NDEF_FILE_OFFSET		0xFFE6
#define NDEF_BUFFER_START		0xFFE4
#define TEST_FUNCTION_REG   	0xFFE2
#define SWTX_INDEX				0xFFDE
#define CUSTOM_RESPONSE_REG		0xFFDA

//define the different virtual register bits
//CONTROL_REG bits
#define SW_RESET			BIT0
#define RF_ENABLE			BIT1
#define INT_ENABLE			BIT2
#define INTO_HIGH			BIT3
#define INTO_DRIVE			BIT4
#define BIP8_ENABLE			BIT5
#define STANDBY_ENABLE		BIT6
#define AUTO_ACK_ON_WRITE	BIT8

//STATUS_REG bits
#define READY					BIT0
#define CRC_ACTIVE				BIT1
#define RF_BUSY					BIT2
#define APP_STATUS_REGS			BIT4 + BIT5 + BIT6
#define FILE_SELECT_STATUS		BIT4
#define FILE_REQUEST_STATUS		BIT5
#define FILE_AVAILABLE_STATUS	BIT5 + BIT4


//INT_ENABLE_REG bits
#define EOR_INT_ENABLE				BIT1
#define EOW_INT_ENABLE				BIT2
#define CRC_INT_ENABLE				BIT3
#define BIP8_ERROR_INT_ENABLE		BIT4
#define DATA_TRANSACTION_INT_ENABLE	BIT5
#define FIELD_REMOVED_INT_ENABLE 	BIT6
#define GENERIC_ERROR_INT_ENABLE	BIT7
#define EXTRA_DATA_IN_INT_ENABLE	BIT8

//INT_FLAG_REG bits
#define EOR_INT_FLAG				BIT1
#define EOW_INT_FLAG				BIT2
#define CRC_INT_FLAG				BIT3
#define BIP8_ERROR_INT_FLAG			BIT4
#define DATA_TRANSACTION_INT_FLAG	BIT5
#define FIELD_REMOVED_INT_FLAG	 	BIT6
#define GENERIC_ERROR_INT_FLAG		BIT7
#define EXTRA_DATA_IN_FLAG			BIT8

//COMM_WD_CTRL_REG bits
#define WD_ENABLE	BIT0
#define TIMEOUT_PERIOD_2_SEC	0
#define TIMEOUT_PERIOD_32_SEC	BIT1
#define TIMEOUT_PERIOD_8_5_MIN	BIT2
#define TIMEOUT_PERIOD_MASK		BIT1 + BIT2 + BIT3


//Host response index
#define INT_SERVICED_FIELD 			BIT0
#define FILE_EXISTS_FIELD			BIT1
#define CUSTOM_RESPONSE_FIELD		BIT2
#define EXTRA_DATA_IN_SENT_FIELD	BIT3
#define FILE_DOES_NOT_EXIST_FIELD	0

#define RF430_DEFAULT_DATA		{  														\
/*NDEF Tag Application Name*/ 															\
0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 												\
																						\
/*Capability Container ID*/ 															\
0xE1, 0x03, 																			\
0x00, 0x0F,	/* CCLEN */																	\
0x20,		/* Mapping version 2.0 */													\
0x00, 0xF9,	/* MLe (49 bytes); Maximum R-APDU data size */								\
0x00, 0xF6, /* MLc (52 bytes); Maximum C-APDU data size */								\
0x04, 		/* Tag, File Control TLV (4 = NDEF file) */									\
0x06, 		/* Length, File Control TLV (6 = 6 bytes of data for this tag) */			\
0xE1, 0x04,	/* File Identifier */														\
0x0B, 0xDF, /* Max NDEF size (3037 bytes of useable memory) */							\
0x00, 		/* NDEF file read access condition, read access without any security */		\
0x00, 		/* NDEF file write access condition; write access without any security */	\
																						\
/* NDEF File ID */ 																		\
0xE1, 0x04, 																			\
																						\
/* NDEF File for Hello World */                             				            \
0x00, 0x14, /* NLEN: NDEF length (20 byte long message, max. length for RF430CL) */     \
                                                                                        \
/* NDEF Record (refer to NFC Data Exchange Format specifications)*/                     \
0xD1,       /*MB(Message Begin), SR(Short Record) flags set, ME(Message End), IL(ID length field present) flags cleared; TNF(3bits) = 1; */ \
0x01, 0x10, /*Type Length = 0x01; Payload Length = 0x10 */                              \
0x54,       /* Type = T (text) */                                                       \
0x02,       /* 1st payload byte: "Start of Text", as specified in ASCII Tables */       \
0x65, 0x6E, /* 'e', 'n', (2nd, 3rd payload bytes*/                                      \
                                                                                        \
/* 'Hello, world!' NDEF data*/                                                          \
0x48, 0x65, 0x6C, 0x6C, 0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21            \
	} /* End of data */

/********************************************************************************************/

typedef struct NdefFile_Type
{
	unsigned char FileID[2];		// The NFC file ID
	unsigned char * FilePointer;	// the location in MCU memory where it is located
	unsigned int FileLength;		// the length of the file
}NdefFileType;

extern unsigned char FileTextE104[];	//NFC NDEF File

enum FileExistType
{
	FileFound 		= 1,
	FileNotFound 	= 0
};

enum FileExistType SearchForFile(uint8_t *fileId);

uint16_t SendDataOnFile(uint16_t selectedFile, uint16_t buffer_start, uint16_t file_offset, uint16_t length);
void ReadDataOnFile(uint16_t selectedFile, uint16_t buffer_start, uint16_t file_offset, uint16_t length);
#endif
