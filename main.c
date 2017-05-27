#include <driverlib.h>

#include "myclocks.h"
#include "tmp112.h"
#include "rf430nfc.h"
#include "rf430Process.h"
//#include "myuart.h"
//#include "myTimers.h"
#include "rtc.h"
#include "datalog.h"

#define TEMP_RECORD_THRSHLD 500

//#define DEBUG 1

//Temp_Modes_t g_ui8TemperatureModeFlag;
unsigned char ui8TemperatureNegFlag;

unsigned int g_TempDataFahr;
unsigned int g_TempDataCel;

char g_TempNegFlagCel = 0;
char g_TempNegFlagFahr = 0;

typedef enum {
	Fahrenheit = 0, Celcius
} Temp_Modes_t;

Temp_Modes_t g_ui8TemperatureModeFlag = Celcius;

unsigned char tempFired = 0;
unsigned char nfcFired = 0;

//***** Prototypes ************************************************************
void initGPIO(void);

int main(void) {

	unsigned int Temperature;		//to hold the temperature
	unsigned int flags = 0;	// to hold the interuupt flag of nfc ic

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	/*************************initialize the hardware**************************/
	initGPIO();
	initClocks();				// set aclk 10K, smclk 4M, mclk 4M\

#ifdef DEBUG
	myuart_init();				// at 9600 baud
#endif
	RTC_init();					//initialize rtc
	GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN5);
	datalog_Init();				//initialize datalogger setting
	//initTimers();				// for 6.5sec temperature reads
	TMP_Config_Init();			// configure to be in shutdown one shot mode
	RF430_Init();				// resets the nfc ic
	GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN5);
#ifdef DEBUG
	myuart_tx_string("Program started...\r\n");
#endif
	while (1) {
		__bis_SR_register(LPM4_bits + GIE); //go to low power mode and enable interrupts. We are waiting for an NFC read or write of/to the RF430
		__no_operation();
/*************************************************************************************************************************************************/
/********************************nfc code*********************************************************************************************************/
 /***************************************************************************************************************************************************/
		if (nfcFired)/* (!(P2IN & BIT2))*/{	//if int from the nfc ic

			flags = Read_Register(INT_FLAG_REG); //read the flag register to check if a read or write

			GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN5);
			GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN5);
			do {
				if (flags) {
					rf430Interrupt(flags);
				}
				flags = Read_Register(INT_FLAG_REG);
			} while (flags);

			GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN5);

			flags = 0;
			nfcFired = 0;
			P2IFG &= ~BIT2;	//clear the interrupt again
			P2IE |= BIT2;	//enable the interrupt
		}

/********************************************************************************************************************************************************/
/******************************************Temperature code *********************************************************************************************
 *******************************************************************************************************************************************************/
		if (tempFired) {
			tempFired = 0;
			GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN6);
			TMP_Get_Temp(&Temperature, &ui8TemperatureNegFlag,
					g_ui8TemperatureModeFlag);
			if (ui8TemperatureNegFlag) {
				Temperature = (-1.0) * Temperature;	//think shoud change to signed variable
			}

			if(Temperature > TEMP_RECORD_THRSHLD ){
				data_buffer(Temperature);
			}
			else{
				GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN6);
#ifdef	DEBUG
				myuart_tx_string("it is so warm,,, going to sleep...");
#endif
				__bic_SR_register(LPM4_bits + GIE);
				__no_operation();
			}
			GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN6);
		}

/*******************************************************************************************************************************************************************/
	}

}

void initGPIO(void) {
	//setting unused pins to low

	P1DIR |= 0b00110111;
#ifdef DEBUG
	P2DIR |= 0b10111000;
#else
	P2DIR |= 0b10111011;
#endif
	P3DIR |= 0b11001111;
	P4DIR |= 0xFF;
	PJDIR |= 0b11001111;

	P1OUT = 0x00;
	P2OUT = 0x00;
	P3OUT = 0x00;
	P4OUT = 0x00;
	PJOUT = 0X00;


	GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_PIN6);		//for powering the IIC
	GPIO_setOutputHighOnPin( GPIO_PORT_P2, GPIO_PIN6);

	GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN6);         // d1 led
	GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN6);

	GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN5);         // d2 led
	GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN5);

	P1SEL1 |= BIT6;		//setting p1.6 as sda
	P1SEL1 |= BIT7;		//setting p1.7 as scl


	PMM_unlockLPM5();

}

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
	//INTO interrupt fired
	if (P2IFG & BIT2) {
		P2IE &= ~BIT2; //disable INTO
		P2IFG &= ~BIT2; //clear interrupt flag
		nfcFired = 1;
		__bic_SR_register_on_exit(LPM4_bits + GIE); //wake up to handle INTO
	}
}

////*****************************************************************************
//// Interrupt Service Routine
////*****************************************************************************
//#pragma vector=TIMER1_A1_VECTOR
//__interrupt void timer1_ISR(void) {
//
//	//**************************************************************************
//	// 4. Timer ISR and vector
//	//**************************************************************************
//	switch (__even_in_range(TA1IV, TA1IV_TAIFG)) {
//	case TA1IV_NONE:
//		break;                 // (0x00) None
//	case TA1IV_TACCR1:                      // (0x02) CCR1 IFG
//		_no_operation();
//		break;
//	case TA1IV_TACCR2:                      // (0x04) CCR2 IFG
//		_no_operation();
//		break;
//	case TA1IV_3:
//		break;                    // (0x06) Reserved
//	case TA1IV_4:
//		break;                    // (0x08) Reserved
//	case TA1IV_5:
//		break;                    // (0x0A) Reserved
//	case TA1IV_6:
//		break;                    // (0x0C) Reserved
//	case TA1IV_TAIFG:             // (0x0E) TA1IFG - TAR overflow
//		tempFired = 1;
//		__bic_SR_register_on_exit(LPM4_bits + GIE); //wake up to handle INTO
//		break;
//	default:
//		_never_executed();
//	}
//}



