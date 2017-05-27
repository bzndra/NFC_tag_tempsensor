/*
 * myuart.h
 *
 *  Created on: Oct 19, 2016
 *      Author: bZn
 */

#ifndef MYUART_H_
#define MYUART_H_

#define UART_TX_PIN BIT0
#define UART_RX_PIN BIT1

void myuart_init(void);
void myuart_tx_byte(unsigned char data);
void myuart_tx_string(char *str);

#endif /* MYUART_H_ */
