#include "msp430.h"
#include "tmp112.h"

void TMP_I2C_Init(void){
		UCB0CTL1 |= UCSWRST;	            			//Software reset enabled
		UCB0CTLW0 |= UCMODE_3  + UCMST + UCSYNC + UCTR;	//I2C mode, Master mode, sync, transmitter
		UCB0CTLW0 |= UCSSEL_2;                    		// SMCLK = 4MHz

		UCB0BRW = 10; 									// Baudrate = SMLK/40 = 400kHz

		UCB0I2CSA  = TMP112_I2C_ADDR;					// Set Slave Address
		UCB0IE = 0;
		UCB0CTL1 &= ~UCSWRST;


}

void TMP_Config_Init(){
	TMP_I2C_Init();

	UCB0CTL1  |= UCSWRST;
	UCB0CTLW1 = UCASTP_2;  // generate STOP condition.
	UCB0TBCNT = 0x0001;
	UCB0CTL1  &= ~UCSWRST;

	UCB0CTL1 |= UCTXSTT + UCTR;		// Start i2c write operation
	while(!(UCB0IFG & UCTXIFG0));
	UCB0TXBUF = TMP112_CONF_REG & 0xFF;
	while(!(UCB0IFG & UCBCNTIFG));

	UCB0CTLW1 = UCASTP_2;  			// generate STOP condition.
	UCB0TBCNT = 0x0002;
	UCB0CTL1 |= UCTXSTT; 			// Repeated start

	UCB0TXBUF = 0x78 & 0xFF;
	while(!(UCB0IFG & UCBCNTIFG));

	UCB0TXBUF = 0xA0 & 0xFF;
	while(!(UCB0IFG & UCBCNTIFG));

	UCB0CTL1  |= UCSWRST;

}

unsigned int getTemperature(){
	unsigned int pTempData;
	unsigned char ui8RxData[2];

		UCB0CTL1  |= UCSWRST;
		UCB0CTLW1 = UCASTP_2;  // generate STOP condition.
		UCB0TBCNT = 0x0001;
		UCB0CTL1  &= ~UCSWRST;

		UCB0CTL1 |= UCTXSTT + UCTR;		// Start i2c write operation
		while(!(UCB0IFG & UCTXIFG0));
		UCB0TXBUF = TMP112_CONF_REG & 0xFF;
		while(!(UCB0IFG & UCBCNTIFG));

		UCB0CTLW1 = UCASTP_2;  			// generate STOP condition.
		UCB0TBCNT = 0x0002;
		UCB0CTL1 |= UCTXSTT; 			// Repeated start

		UCB0TXBUF = 0xF8 & 0xFF;
		while(!(UCB0IFG & UCBCNTIFG));

		UCB0TXBUF = 0xA0 & 0xFF;
		while(!(UCB0IFG & UCBCNTIFG));

		UCB0CTL1  |= UCSWRST;

				UCB0CTL1  |= UCSWRST;
	    		UCB0CTLW1 = UCASTP_2;  // generate STOP condition.
	    		UCB0TBCNT = 0x0001;
	    		UCB0CTL1  &= ~UCSWRST;

	    		UCB0CTL1 |= UCTXSTT + UCTR;		// Start i2c write operation
	    		while(!(UCB0IFG & UCTXIFG0));
	    		UCB0TXBUF = TMP112_TEMP_REG & 0xFF;
	    		while(!(UCB0IFG & UCBCNTIFG));

	    		UCB0CTL1 &= ~UCTR;
	    		UCB0CTLW1 = UCASTP_2;  			// generate STOP condition.
	    		UCB0TBCNT = 0x0002;
	    		UCB0CTL1 |= UCTXSTT; 			// Repeated start

	    		while(!(UCB0IFG & UCRXIFG0));
	    		ui8RxData[1] = UCB0RXBUF;
	    		while(!(UCB0IFG & UCRXIFG0));
	    		ui8RxData[0] = UCB0RXBUF;
	    		while (!(UCB0IFG & UCSTPIFG));  // Ensure stop condition got sent
	    		UCB0CTL1  |= UCSWRST;

	    		pTempData = (0x0FF0 & (ui8RxData[1] << 4)) | (0x0F & (ui8RxData[0] >> 4 ));
	    		pTempData = 0x07FF & pTempData;

	return pTempData;

}

void TMP_Get_Temp(unsigned int* ui16TempReturn, unsigned char* uc8NegFlagReturn, unsigned char uc8ModeFlag ){

	unsigned long pTempData;

	TMP_I2C_Init();

	g_TempNegFlagFahr = 0;

	pTempData = getTemperature();

	if(!(pTempData & 0x800)){ // Sign bit.  If +
		g_TempDataCel = (pTempData * 625)/100;
		g_TempNegFlagCel = 0;					//defined in main
	}  //else is -
	else{
		pTempData = (~pTempData) & 0xFFF;
		g_TempDataCel = (pTempData * 625)/100;
		g_TempNegFlagCel = 1;
	}
/////////////////negative fahreinheit left to do///////////////////////////////
	if(g_TempNegFlagCel){
		g_TempDataFahr  = 320 - ((g_TempDataCel * 9)/5);

		if(g_TempDataCel > 176){  // 176 == 17.6 C == 0 F
			g_TempNegFlagFahr = 1;
			g_TempDataFahr = (~g_TempDataFahr) + 1;
		}
	}
	else{
		g_TempDataFahr  = 320 + ((g_TempDataCel * 9)/5);
	}
//////////////////////////////////////////////////////////////////////////////
	if(uc8ModeFlag){ // Celcius
		*ui16TempReturn = g_TempDataCel;
		*uc8NegFlagReturn = g_TempNegFlagCel;
	}
	else{	//Fahrenheit
		*ui16TempReturn = g_TempDataFahr;
		*uc8NegFlagReturn = g_TempNegFlagFahr;
	}
}

