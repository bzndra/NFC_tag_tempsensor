/*
 * datalog.h
 *
 *  Created on: Nov 1, 2016
 *      Author: kiran
 */

#ifndef DATALOG_H_
#define DATALOG_H_
#include "rtc.h"
#include "TMP112.h"

typedef struct datalog_interval_type {

	unsigned int temp_interval_minute;
	unsigned int numberOfLogs;

}datalog_interval_type;

void datalog_Init();
void data_buffer(unsigned int Temperature);

#endif /* DATALOG_H_ */
