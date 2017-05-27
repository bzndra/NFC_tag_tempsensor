/*
 * myuart.c
 *
 *  Created on: Oct 19, 2016
 *      Author: bZn
 */

#include "msp430.h"
#include "myuart.h"
#include <stdint.h>

void myuart_init(void)
{
  // Configure GPIO
  P2SEL1 |= UART_TX_PIN | UART_RX_PIN;                    // USCI_A0 UART operation
  P2SEL0 &= ~(UART_TX_PIN | UART_RX_PIN);

//  // Disable the GPIO power-on default high-impedance mode to activate
//  // previously configured port settings
//  PM5CTL0 &= ~LOCKLPM5;

//  // Startup clock system with max DCO setting ~8MHz
//  CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
//  CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
//  CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
//  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
//  CSCTL0_H = 0;                             // Lock CS registers

  // Configure USCI_A0 for UART mode
  UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
  UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
  // Baud Rate calculation
  // 8000000/(16*9600) = 52.083  or if smclk = 2Mhz, 2000000/(16*9600)
  // Fractional portion = 0.083
  // User's Guide Table 21-4: UCBRSx = 0x04
  // UCBRFx = int ( (52.083-52)*16) = 1
  UCA0BR0 = 26;                             // 4000000/16/9600
  UCA0BR1 = 0x00;
  UCA0MCTLW |= UCOS16 | UCBRF_1;
  UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
}

void myuart_tx_byte(unsigned char data)
{
	while(!(UCA0IFG&UCTXIFG));
	UCA0TXBUF = data;
}


/*to transmit string characterwise untill null character is found*/
void myuart_tx_string(char *str)
{
	int i = 0;
	while(str[i]!='\0')
	{
		myuart_tx_byte(str[i]);
		i++;
	}

}
#if 0
void myuart_rx()
{
	UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
	__bis_SR_register(LPM3_bits | GIE);       // Enter LPM3, interrupts enabled
	__no_operation();                         // For debugger
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA0IFG&UCTXIFG));
      UCA0TXBUF = UCA0RXBUF;		//echo rxdata
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}
#endif
