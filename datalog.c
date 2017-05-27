/*
 * datalog.c
 *
 *  Created on: Nov 1, 2016
 *      Author: kiran
 */


#include "datalog.h"
#include "rtc.h"
#include "myuart.h"

#define DATA_WIDTH 24

#define MAX_LOGS 500
#define CHECK_INTERVAL 10

#include <stdio.h>

//#define DEBUG 1

#pragma PERSISTENT (numOfLogsInFram)
extern unsigned int numOfLogsInFram = 0;

#pragma PERSISTENT (ui16nlenhold)
extern unsigned int ui16nlenhold = 0x000A;

#pragma PERSISTENT (ui16plenhold)
extern unsigned int ui16plenhold = 0x0003;

datalog_interval_type interval;

extern uint8_t FileTextE104[];
extern rtcType timestamp;

extern unsigned char check;

unsigned char bufferHold[DATA_WIDTH];

void datalog_Init(){
	interval.temp_interval_minute = CHECK_INTERVAL;
	bufferHold[0] = ' ';
	bufferHold[3] = '.';
	bufferHold[6] = ',';
	bufferHold[9] = ':';
	bufferHold[12] = ',';
	bufferHold[17] = '/';
	bufferHold[20] = '/';
	bufferHold[23] = 0x0D;
}


void data_buffer(unsigned int Temperature){
/////////////filling the temperature/////////////////////////
	unsigned int temp;
	temp = Temperature;

#ifdef DEBUG
			char str[30];
			sprintf(str, "\n\rTemperature: %d ", Temperature);
			myuart_tx_string(str);
			myuart_tx_byte(0xB0);
			myuart_tx_byte('C');
			myuart_tx_byte(0x0D);
#endif

	temp = temp / 1000;			//buffer hold 0 has character t
	//FileTextE104[10] = (char) temp + 48;
	bufferHold[1] = (char) temp + 48;
	temp = Temperature % 1000;

	temp = temp / 100;
	//FileTextE104[11] = (char) temp + 48;
	bufferHold[2] = (char) temp + 48;
	temp = Temperature % 100;

	temp = temp / 10;
	//FileTextE104[13] = (char) temp + 48;
	bufferHold[4] = (char) temp + 48;
	temp = Temperature % 10;

	//FileTextE104[14] = (char) temp + 48;
	bufferHold[5] = (char) temp + 48;
//////////////////////////////////////////////////////////////

	timestamp = getTimeStamp();

	/*FileTextE104[16]*/ bufferHold[7] = timestamp.hour[1];
	/*FileTextE104[17]*/ bufferHold[8] = timestamp.hour[0];

	/*FileTextE104[19]*/ bufferHold[10] = timestamp.minute[1];
	/*FileTextE104[20]*/ bufferHold[11] = timestamp.minute[0];

	/*FileTextE104[22]*/ bufferHold[13] = '2';
	/*FileTextE104[23]*/ bufferHold[14] = '0';
	/*FileTextE104[24]*/ bufferHold[15] = timestamp.year[1];
	/*FileTextE104[25]*/ bufferHold[16] = timestamp.year[0];

	/*FileTextE104[27]*/ bufferHold[18] = timestamp.month[1];
	/*FileTextE104[28]*/ bufferHold[19] = timestamp.month[0];

	/*FileTextE104[30]*/ bufferHold[21]  = timestamp.day[1];
	/*FileTextE104[31]*/ bufferHold[22] = timestamp.day[0];


#ifdef DEBUG
	myuart_tx_byte(timestamp.hour[1]);
	myuart_tx_byte(timestamp.hour[0]);

	myuart_tx_byte(':');

	myuart_tx_byte(timestamp.minute[1]);
	myuart_tx_byte(timestamp.minute[0]);

	myuart_tx_byte(0x20);

	myuart_tx_byte(timestamp.year[1]);
	myuart_tx_byte(timestamp.year[0]);
	myuart_tx_byte('/');
	myuart_tx_byte(timestamp.month[1]);
	myuart_tx_byte(timestamp.month[0]);
	myuart_tx_byte('/');
	myuart_tx_byte(timestamp.day[1]);
	myuart_tx_byte(timestamp.day[0]);
	myuart_tx_byte(0x0D);
#endif

	if(ui16nlenhold <  DATA_WIDTH*MAX_LOGS){	//maximum data it can hold

		ui16nlenhold += DATA_WIDTH;

		temp = ui16nlenhold;
	/////setting up the length of the ndef record
		FileTextE104[1] = ( char) ui16nlenhold;
		temp >>=8;
		FileTextE104[0] = ( char) temp;
	/////setting up the length of the ndef payload
		ui16plenhold += DATA_WIDTH;
		temp = ui16plenhold;

		FileTextE104[7] = ( char) ui16plenhold;
		temp >>= 8;
		FileTextE104[6] = ( char) temp;

		for( temp = 0 ; temp < DATA_WIDTH ; temp++){
			FileTextE104[12 + temp + numOfLogsInFram*DATA_WIDTH] = bufferHold[temp];
		}

		numOfLogsInFram+=1;

	} else {
#ifdef DEBUG
		myuart_tx_string("\n\r.............Memory full..............\n\r");
#endif
		for(temp = 12 ;temp < MAX_LOGS*DATA_WIDTH - DATA_WIDTH ; temp++){
			FileTextE104[temp] = FileTextE104[temp + DATA_WIDTH];
		}

		for( temp = 0 ; temp < DATA_WIDTH ; temp++){
			FileTextE104[12 + temp + (numOfLogsInFram-1)*DATA_WIDTH] = bufferHold[temp];
		}

	}





#ifdef DEBUG

		sprintf(str,"\n\rTL=%d dumping all\n\r",numOfLogsInFram);
		myuart_tx_string(str);
//		for(temp = 0 ;temp < numOfLogsInFram*DATA_WIDTH;temp++){
//				myuart_tx_byte(FileTextE104[12+temp]);
//		}

#endif


}

