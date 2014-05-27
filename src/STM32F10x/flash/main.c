#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "mini-printf.h"

static __IO uint32_t TimingDelay;

RCC_ClocksTypeDef RCC_Clocks;

// uart2
// PA2
// PA3

// led
// PA5

void usart_write(uint8_t ch)
{
  USART_SendData(USART2, (uint8_t) ch);
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {
  }
}

uint8_t usart_read(void){
  while ( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
  
  return (uint8_t)USART_ReceiveData(USART2);
}

uint8_t usart_available(void)
{
	if ( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET )
	 return 1;

  return 0;		
}

void usart_print( char *msg )
{
	int len = strlen( msg );

	for ( int c = 0; c < len; c++ )
		usart_write( (uint8_t)*msg++ );
}


int main(void)
{
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
    
  GPIO_InitTypeDef gpio_init;
  USART_InitTypeDef usart_init;
  USART_ClockInitTypeDef usart_clk_init;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
 
  // PA2 = Tx, PA3 = Rx
  gpio_init.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &gpio_init);

  gpio_init.GPIO_Pin = GPIO_Pin_5;
  gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &gpio_init);

  GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
 
  USART_ClockStructInit(&usart_clk_init);
  USART_ClockInit(USART2, &usart_clk_init);
 
  usart_init.USART_BaudRate =            9600;
  usart_init.USART_WordLength =          USART_WordLength_8b;
  usart_init.USART_StopBits =            USART_StopBits_1;
  usart_init.USART_Parity =              USART_Parity_No ;
  usart_init.USART_Mode =                USART_Mode_Rx | USART_Mode_Tx;
  usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART2, &usart_init);
  USART_Cmd(USART2,ENABLE);    
     
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) {}       

  uint32_t flashaddr = 0x0800010C;
 
  /*  
  FLASH_Unlock();
  FLASH_Status error = FLASH_ErasePage( flashaddr);
  FLASH_ProgramWord( flashaddr, 0x54534554);
  FLASH_Lock();
  */
  
  char buffer[20];
  while (1)
  {
    uint32_t value = *((uint32_t*)flashaddr);

    //uint8_t *ch = (uint8_t*)flashaddr;

    snprintf(buffer, 20, "%d", value );
    usart_print( buffer );
//    usart_write(*ch++);
//    usart_write(*ch++);
//    usart_write(*ch++);
//    usart_write(*ch);

    // toggle led
    GPIOA->ODR ^= GPIO_Pin_5;
    Delay(1000);
  }
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  
  while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
