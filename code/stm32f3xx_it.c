/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"

/* USER CODE BEGIN 0 */

#include "byte_fifo.h"

extern Byte_fifo usart1_rx_fifo;
extern Byte_fifo usart1_tx_fifo;

void usart4_transmit_interrupt_enable()
{
	SET_BIT(USART1->CR1, USART_CR1_TXEIE);
}

void usart4_receive_interrupt_enable()
{
	SET_BIT(USART1->CR1, USART_CR1_RXNEIE);
}

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

extern TIM_HandleTypeDef htim2;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
*/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

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
	
	if((isrflags & USART_ISR_TXE != FLAGS_CLEAR)
			&& ((cr1its & USART_CR1_TXEIE != FLAGS_CLEAR))) {

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

  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

    return;
  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
