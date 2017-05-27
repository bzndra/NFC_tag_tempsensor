/*
 * rtc.h
 *
 *  Created on: Oct 31, 2016
 *      Author: kiran
 */

#ifndef RTC_H_
#define RTC_H_

#include "msp430.h"
#include "stdint.h"
#include "datalog.h"


extern unsigned char HOURS;
extern unsigned char MINUTES;
extern unsigned char MONTHS;
extern unsigned char DAYS;
extern unsigned int YEARS;

typedef struct rtc_Type
{
	unsigned char hour[2];
	unsigned char minute[2];
	unsigned char year[2];
	unsigned char day[2];
	unsigned char month[2];

}rtcType;

rtcType timestamp;

void RTC_init();
rtcType getTimeStamp();


#endif /* RTC_H_ */
