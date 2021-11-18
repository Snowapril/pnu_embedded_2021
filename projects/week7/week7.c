#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);

void EXTI15_10_IRQHandler(void);

void Delay(void);

void sendDataUART1(uint16_t data);

//---------------------------------------------------------------------------------------------------
int mode = 0; // 0 = A, 1 = B
int print = 0;
unsigned int led_idx = 0; // 0~3
char msg[] = "Hello Team02\r\n";

void RCC_Configure(void) // stm32f10x_rcc.h 참고
{
	// TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
	
	/* UART TX/RX port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* JoyStick Up/Down port clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        
	/* LED port clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	/* USART1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) // stm32f10x_gpio.h 참고
{
    GPIO_InitTypeDef GPIO_InitStructure;

	// TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
	
    /* JoyStick up, down pin setting  UP(5), DOWN(2)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* button pin setting */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* LED pin setting*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
    /* UART pin setting */
   GPIO_InitTypeDef GPIO_InitStructure_USART1TX;
   GPIO_InitTypeDef GPIO_InitStructure_USART1RX;
   
   GPIO_InitStructure_USART1TX.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_InitStructure_USART1TX.GPIO_Pin = (GPIO_Pin_9);
   GPIO_InitStructure_USART1TX.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure_USART1TX);

   GPIO_InitStructure_USART1RX.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
   GPIO_InitStructure_USART1RX.GPIO_Pin = (GPIO_Pin_10);
   GPIO_InitStructure_USART1RX.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure_USART1RX);
}

void EXTI_Configure(void) // stm32f10x_gpio.h 참고
{
    EXTI_InitTypeDef EXTI_InitStructure;

	// TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
	// TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'
	
    /* Joystick Down */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Joystick Up */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Button */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    
	
	// NOTE: do not select the UART GPIO pin used as EXTI Line here
}

void USART1_Init(void) // stm32f10x_usart.h 참고
{
        /*TODO: USART1 configuration*/
   USART_InitTypeDef USART_InitStructure;

      /*TODO: USART1 cmd ENABLE*/
   USART_Cmd(USART1, ENABLE);
   
   USART_InitStructure.USART_BaudRate = 9600;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

   USART_Init(USART1, &USART_InitStructure);

  

   /*TODO: USART1 IT Config*/
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void NVIC_Configure() {
   /*TODO: NVIC_configuration */
   NVIC_InitTypeDef NVIC_InitStructure_UP;
   NVIC_InitTypeDef NVIC_InitStructure_DOWN;
   NVIC_InitTypeDef NVIC_InitStructure_BUTT;
   NVIC_InitTypeDef NVIC_InitStructure_UART;

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

   NVIC_InitStructure_UP.NVIC_IRQChannel = EXTI2_IRQn;
   NVIC_InitStructure_UP.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure_UP.NVIC_IRQChannelPreemptionPriority = 0x01;
   NVIC_InitStructure_UP.NVIC_IRQChannelSubPriority = 0x00;
   NVIC_Init(&NVIC_InitStructure_UP);

   NVIC_InitStructure_DOWN.NVIC_IRQChannel = EXTI9_5_IRQn;
   NVIC_InitStructure_DOWN.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure_DOWN.NVIC_IRQChannelPreemptionPriority = 0x02;
   NVIC_InitStructure_DOWN.NVIC_IRQChannelSubPriority = 0x00;
   NVIC_Init(&NVIC_InitStructure_DOWN);

   NVIC_InitStructure_BUTT.NVIC_IRQChannel = EXTI15_10_IRQn;
   NVIC_InitStructure_BUTT.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure_BUTT.NVIC_IRQChannelPreemptionPriority = 0x03;
   NVIC_InitStructure_BUTT.NVIC_IRQChannelSubPriority = 0x00;
   NVIC_Init(&NVIC_InitStructure_BUTT);

   NVIC_EnableIRQ(USART1_IRQn);
   NVIC_InitStructure_UART.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure_UART.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure_UART.NVIC_IRQChannelPreemptionPriority = 0x00;
   NVIC_InitStructure_UART.NVIC_IRQChannelSubPriority = 0x00;
   NVIC_Init(&NVIC_InitStructure_UART);
}

void USART1_IRQHandler() {
	char word;
        printf("asfasfd\n");
        if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
    	// the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);
    
        // TODO implement, 키보드 입력 'd' 또는 'u'에 따라 동작
        if (word == 'u') {
          // A 동작
          mode = 0;
        } else if (word == 'd') {
          // B 동작
          mode = 1;
        }
        // clear 'Read data register not empty' flag
    	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}

void EXTI15_10_IRQHandler(void) { // when the button is pressed

	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		/* send UART 1 */
                    print = 1;
        EXTI_ClearITPendingBit(EXTI_Line11);
	}
}


// TODO: Create Joystick interrupt handler functions
void EXTI2_IRQHandler(void) {
  // up
  if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
		
                  mode = 0;
        EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void EXTI9_5_IRQHandler(void) {
  // down
  if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
		
                mode = 1;
        EXTI_ClearITPendingBit(EXTI_Line5);
	}
}

void Delay(void) {
	int i;

	for (i = 0; i < 2000000; i++) {}
}

void sendDataUART1(uint16_t data) {
	USART_SendData(USART1, data);
	/* Wait till TC is set */
	while ((USART1->SR & USART_SR_TC) == 0);
}

int main(void)
{

    SystemInit();

    RCC_Configure();

    GPIO_Configure();

    EXTI_Configure();

    USART1_Init();

    NVIC_Configure();

    unsigned led_array[4] = {
      GPIO_Pin_2,
      GPIO_Pin_3,
      GPIO_Pin_4,
      GPIO_Pin_7,
    };
    while (1) {
    	// TODO: implement 
        int index = led_idx % 4;
        for (int i = 0; i < 4; ++i) {
          if (i == index) {
             GPIO_SetBits(GPIOD, led_array[i]);
          } else {
           GPIO_ResetBits(GPIOD, led_array[i]);
         }
        }
        
        if (print) {
          char *tmp = &msg[0];
              while (*tmp != '\0') {
                sendDataUART1(*tmp);
                tmp++;
              }
              print = 0;
        }
          
    	// Delay
        if (mode == 0) {
        GPIO_SetBits(GPIOD, led_array[0]);
        GPIO_ResetBits(GPIOD, led_array[1]);
        led_idx++;
      } else {
        GPIO_SetBits(GPIOD, led_array[1]);
        GPIO_ResetBits(GPIOD, led_array[0]);
        led_idx--;
      }
      
      printf("mode : %d, index : %d\n", mode, index);
      
    }
    return 0;
}
