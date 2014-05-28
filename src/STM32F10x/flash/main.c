#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "mini-printf.h"
#include "md5.h"

#define RX_BUFFER_SIZE  300

// function prototypes
void usart_print( char *msg );

static __IO uint32_t TimingDelay;
static uint8_t rxBuffer[RX_BUFFER_SIZE];
static uint8_t rxCount;

RCC_ClocksTypeDef RCC_Clocks;

// uart2
// PA2
// PA3

// led
// PA5

void usart_printf( char *fmt, ... )
{
  char buffer[256];

  int ret;
  va_list va;
  va_start(va, fmt);
  ret = mini_vsnprintf(buffer, 256, fmt, va);
  va_end(va);  

  usart_print(buffer);
}

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

void usartWrite( uint8_t data[], uint8_t offset, uint8_t count )
{
  for ( uint8_t c = 0; c < count; c++ )
    usart_write( data[offset + c] );
}


void shiftRXBuffer(int count)
{
  memmove( &rxBuffer[0], &rxBuffer[count], RX_BUFFER_SIZE - count );
  rxCount-=count;
}

void makeFrame( uint8_t *buffer[], uint8_t cmd, uint8_t len, uint8_t *data[])
{
  buffer[0] = 0x21;
  buffer[1] = 0x21;
  buffer[2] = cmd;
  buffer[3] = len;

  for ( int c = 0; c < len; c++)
  {
    buffer[4 + c] = data[c];
  }
}

void handlePing()
{
  uint8_t buffer[4];
  makeFrame( &buffer, 0x01, 0x00, NULL);
  usartWrite(buffer, 0, 4);
}

void processCommands(uint8_t cmd, uint8_t len, uint8_t data[])
{
  switch( cmd )
  {
    case 1 : handlePing(); break;
  }
}

void processRX()
{
  if (rxCount < 4)
    return; // need atleast 4 bytes for a command

  while ( rxCount > 3)
  {
    // do we have identifiers?
    if ( rxBuffer[0] != '!' || rxBuffer[1] != '!' )
    {
      // shift one up and try again
      shiftRXBuffer(1);
      continue;
    }

    // get cmd
    uint8_t cmd = rxBuffer[2];
    uint8_t len = rxBuffer[3];

    GPIOA->ODR ^= GPIO_Pin_5;

    //usart_printf("cmd recv: %u\r\n", cmd);
    //usart_printf("data len: %u\r\n", len);

    // get data
    uint8_t dataPacket[len];

    for ( int c = 0; c < len; c++)
      dataPacket[c] = rxBuffer[4 + c];

    usart_write(len);

    processCommands(cmd, len, dataPacket);

    usart_print( "done");
    // remove from buffer
    shiftRXBuffer(4+len);

    usart_print( "done");
  }

}

int main(void)
{
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
    
  GPIO_InitTypeDef gpio_init;
 
  // rx setup **********************************************************************************************
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

  memset(&rxBuffer, 0x00, 300);
  // *******************************************************************************************************   

  uint32_t sanityOffset = 0x10C;
  uint32_t firmlenOffset = sanityOffset + 4;
  
  uint32_t sanityCheck = *((uint32_t*)sanityOffset);
  uint32_t firmwarelen = *((uint32_t*)firmlenOffset);

  //usart_print("Bootloader\r\n");

  if ( sanityCheck != 0xDEADBEEF )
  {
    //usart_print( "Sanity Check Failed\r\n");
    return 1;
  }
  else
  {
    //usart_printf("FW Len: %X\r\n", firmwarelen );
  }

  while (1)
  {
    
      while ( usart_available() && rxCount < 300)
      {
        rxBuffer[rxCount++] = usart_read();
        processRX();
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

