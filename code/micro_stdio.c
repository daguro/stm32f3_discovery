/*
 * Copyright 2018 Daniel G. Robinson
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * @file micro_stdio.c
 * @brief implement stdio functions on micro
 * @author Daniel G. Robinson
 * @date 3 Jul 2018
 */

#ifndef CONSOLE_BUILD
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"
#endif // CONSOLE_BUILD

#include "byte_fifo.h"

#define USART1_RX_BUF_SIZE	200

uint8_t usart1_rx_buf[USART1_RX_BUF_SIZE];

Byte_fifo usart1_rx_fifo = {
	&usart1_rx_buf[0],
	0, 0,
	USART1_RX_BUF_SIZE
};

#define USART1_TX_BUF_SIZE	200

uint8_t usart1_tx_buf[USART1_TX_BUF_SIZE];

Byte_fifo usart1_tx_fifo = {
	&usart1_tx_buf[0],
	0, 0,
	USART1_TX_BUF_SIZE
};

int _write (int fd, const void *buf, int count)
{
	uint8_t ch, *ss;
	int ii;

	ss = (uint8_t*) buf;

	for(ii = 0; ii < count; ii++) {
		ch = ss[ii];
_write_retry:
		if(bf_space_avail(&usart1_tx_fifo)) {
			bf_write(&usart1_tx_fifo, ch);
			extern void usart1_transmit_interrupt_enable();
			usart1_transmit_interrupt_enable();
		}
		else {
			goto _write_retry;
		}
	}

	return count;
}

int micro_putc(int cc)
{
	/*
	 * put the output routine for your system here
	 */
	uint8_t ch;

	ch = (char) cc;
putc_retry:
	if(bf_space_avail(&usart1_tx_fifo)) {
		bf_write(&usart1_tx_fifo, ch);
		extern void usart1_transmit_interrupt_enable();
		usart1_transmit_interrupt_enable();
	}
	else {
		goto putc_retry;
	}

	return 0;
}

int micro_puts(const char *ss)
{
	/*
	 * put the output routine for your system here
	 */
	uint8_t ch;

	while(ss && *ss) {
		ch = *ss++;
puts_retry:
		if(bf_space_avail(&usart1_tx_fifo)) {
			bf_write(&usart1_tx_fifo, ch);
			extern void usart1_transmit_interrupt_enable();
			usart1_transmit_interrupt_enable();
		}
		else {
			goto puts_retry;
		}
	}

	return 0;
}

int micro_getc()
{
	/*
	 * put the input routine for your system here
	 */
	uint8_t ch = 0;

	if(bf_data_avail(&usart1_rx_fifo)) ch = bf_read(&usart1_rx_fifo);

	return (int) ch;
}

char *micro_gets(char *ss, int nn)
{
	/*
	 * put the input routine for your system here
	 */
	return (char*) 0;
}

void usart1_transmit_interrupt_enable()
{
	SET_BIT(USART1->CR1, USART_CR1_TXEIE);
}

void usart1_receive_interrupt_enable()
{
	SET_BIT(USART1->CR1, USART_CR1_RXNEIE);
}


void usart1_irq_handler()
{
		/*
	 * copied from the HAL UART IRQ handler
	 *
	 * USART1 is ((USART_TypeDef *) USART1_BASE), the address of the base of
	 * USART1 registers
	 */
  uint32_t isrflags   = READ_REG(USART1->ISR);		// read interrupt status register
  uint32_t cr1its     = READ_REG(USART1->CR1);		// read control register 1
  uint32_t cr3its;
  uint32_t errorflags;
	
  uint32_t isr_check, cr1_check;

	isr_check = isrflags & USART_ISR_TXE;
	cr1_check = cr1its & USART_CR1_TXEIE;

#define FLAGS_CLEAR (0)

  /* If no error occurs */
  /*
   * look for errors
   * PE == parity error
   * FE == framing error
   * ORE == overrun error
   * NE == noise error, also called Noise Flag in documentation
   *
   */
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  if (errorflags == RESET) {	// RESET == 0, all flags clear
    /* UART in mode Receiver ---------------------------------------------------*/
	  // if RXNE, receive not empty is set and RXNEIE, interrupt enabled set
    if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
		// do receipt of character
		// RDR is read data register, UART_RDR in ref manual
		uint32_t read_byte = USART1->RDR;	
		if(bf_space_avail(&usart1_rx_fifo)) {
			bf_write(&usart1_rx_fifo, (uint8_t) read_byte);
		}
    }
	// if transmit enabled and transmit empty
	

	if(((isrflags & USART_ISR_TXE) != FLAGS_CLEAR)
			&& (((cr1its & USART_CR1_TXEIE) != FLAGS_CLEAR))) {

		if(bf_data_avail(&usart1_tx_fifo)) {
			uint32_t write_byte = (uint32_t) bf_read(&usart1_tx_fifo);
			USART1->TDR = write_byte;
		}
		else {			// transmit fifo emtpy, clear tx enable bit
			CLEAR_BIT(USART1->CR1, USART_CR1_TXEIE);
		}
	}
	return;
  }  

  /*
   * okay, that was the easy stuff.
   * we're here because there were errors
   *
   * I've modified the standard HAL code somewhat.
   * I don't why the HAL uses RESET (== 0) as a name for what is essentially
   * CLEAR.  CLEAR is a better name
   * RESET is an enum in Drivers/CMSIS/Device/ST/STM32F3xx/Include/stm32f3xx.h
   */


  // read control register 3
  
  cr3its = READ_REG(USART1->CR3);

  // check for EIE, error interrupt enable
 
  if((cr3its & USART_CR3_EIE) != RESET) {

	  // if rx not empty and parity error interrupts are enabled
	  
	  if((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET) {
    
		if(((isrflags & USART_ISR_PE) != RESET)
				&& ((cr1its & USART_CR1_PEIE) != RESET)) {
		  USART1->ICR = USART_ICR_PECF;
		}

    /* UART frame error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET)) {
		  USART1->ICR = USART_ICR_FECF;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if(((isrflags & USART_ISR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET)) {
		  USART1->ICR = USART_ICR_NCF;
    }
    
    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if(((isrflags & USART_ISR_ORE) != RESET) &&
       (((cr1its & USART_CR1_RXNEIE) != RESET) || ((cr3its & USART_CR3_EIE) != RESET))) {
		  USART1->ICR = USART_ICR_ORECF;

    }

    /* Call UART Error Call back function if need be --------------------------*/
	// we have errors, but what are we going to do with them?
	// I really don't care, because this is a simple uart connected to the
	// debuggger
	//
    }
  }
}

