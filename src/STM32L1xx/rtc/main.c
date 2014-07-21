// rtc real time clock example on the STM32L152 Nucleo
// note: you need X2 populated with a 32.768 crystal
// read here for more information:
// http://tomvandenbon.co.za/blog/?p=94 

#include "main.h"
#include "mini-printf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
 
  // PA2 = Tx, PA3 = Rx
  gpio_init.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  gpio_init.GPIO_Mode = GPIO_Mode_AF;
  gpio_init.GPIO_Speed = GPIO_Speed_40MHz;
  gpio_init.GPIO_OType = GPIO_OType_PP;
  gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &gpio_init);

  gpio_init.GPIO_Pin = GPIO_Pin_5;
  gpio_init.GPIO_Mode = GPIO_Mode_OUT;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &gpio_init);

 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);  
 
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
 
  usart_print( "RTC Example\r\n" );      
      
  // setup lse for rtc and lcd
  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);  

  /* Reset RTC Backup Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait until LSE is ready */
  usart_print( "Waiting for LSE\r\n" );
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
  usart_print( "LSE Ready\r\n" );
      
   /* RTC Clock Source Selection */ 
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
  
  /* Enable the RTC */
  RCC_RTCCLKCmd(ENABLE); 

  RTC_WaitForSynchro();  

  // init rtc
  RTC_InitTypeDef rtcInit;
  rtcInit.RTC_HourFormat = RTC_HourFormat_24;
  rtcInit.RTC_AsynchPrediv = 0x7F; 
  rtcInit.RTC_SynchPrediv = 0xFF; 
  RTC_Init(&rtcInit); 


  // Set the time (BCD)
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours   = 0x01;
  RTC_TimeStructure.RTC_Minutes = 0x00;
  RTC_TimeStructure.RTC_Seconds = 0x00;
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);


  char strDisp[20];
  int prevSecond = -1;

  while (1)
  {     
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure );

    if ( RTC_TimeStructure.RTC_Seconds != prevSecond )
    {      
      mini_snprintf( strDisp, 20,  "%02d:%02d:%02d\r\n", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds );
      usart_print( strDisp );
      prevSecond = RTC_TimeStructure.RTC_Seconds;
      GPIOA->ODR ^= GPIO_Pin_5;
    }      

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

