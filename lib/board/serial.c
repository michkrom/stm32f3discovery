/******************************************************************************/
/*                                                                            */
/*     Simple serial port I/O routines for the STM32F303 Cortex-M3 ARM MCU      */
/*                                                                            */
/******************************************************************************/

#include "serial.h"

#include "stm32f30x_conf.h"
#include "stm32f30x.h"

#include "stm32f30x_rcc.h"

/* USART2 PA.2 Tx, PA.3 Rx STM32F303 */

int InitSerial(int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);

	/* Connect PXx to USARTx_Rx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART configuration */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);	
  return 0;
}
<<<<<<< HEAD
=======

//******************************************************************************
// Hosting of stdio functionality through USART1
//******************************************************************************
 
#include <stdio.h>

#ifdef UVISION
#include <rt_misc.h>
 
#pragma import(__use_no_semihosting_swi)
 
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
 
int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
  	
  return(ch);
}
 
int fgetc(FILE *f)
{
  char ch;
  while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET); 
  ch = USART_ReceiveData(USART2);
	return((int)ch);
}
 
int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}
 
void _ttywrch(int ch)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
  USART_SendData(USART2, ch);
}
#elseifdef NEWLIB

  
#endif
>>>>>>> d7c7073da17cfc360f35d3ebd6555c81e3cfc5cb
