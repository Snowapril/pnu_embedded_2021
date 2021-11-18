#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "lcd.h"
#include "touch.h"
#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);
void ADC_Configure(void);
void Delay(void);

int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};
//---------------------------------------------------------------------------------------------------
uint16_t value = 0;
uint16_t pos_x = 0;
uint16_t pos_y = 0;
uint16_t tim2_counter = 0;
vu32 ADC_Value;

void RCC_Configure(void) // stm32f10x_rcc.h Âü°í
{
	/* ADC1 Clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void set_PC1(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void set_PC2(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void ADC_Configure(void) {
  
  ADC_InitTypeDef ADC_InitStructure; 
  ADC_DeInit(ADC1); 
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 2;
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_55Cycles5);
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
}

void ADC_start(void) {
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void Timer_Configure(void) {
   NVIC_InitTypeDef NVIC_InitStructure;// 타이머 인터럽트 컨드롤
   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//타이머
   TIM_OCInitTypeDef OutputChannel;   
    
   /* Enable TIM2 Global Interrupt */
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
   NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

   NVIC_Init(&NVIC_InitStructure);//NVIC 초기화
   
   /* TIM2 Initialize */  
   // 1초마다 타이머가 돌도록 Period와 Prescale 설정
   TIM_TimeBaseStructure.TIM_Period=1200; // 100kHz// 주기
   TIM_TimeBaseStructure.TIM_Prescaler=60000; //
   TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;

   TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//타임 초기화
   
   
   /* TIM2 Enale */
   TIM_ARRPreloadConfig(TIM2, ENABLE);
   TIM_Cmd(TIM2, ENABLE);
   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void DMA_Configure(void) {
    DMA_InitTypeDef DMA_InitStructure; 
    DMA_DeInit(DMA1_Channel1); 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR; 
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &ADC_Value; 
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
    DMA_InitStructure.DMA_BufferSize = 2; 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void GPIO_Configure(void) // stm32f10x_gpio.h Âü°í
{
    GPIO_InitTypeDef GPIO_InitStructure;

	// TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
	/* Configure ADC_Input pins */
  GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0      // ADC12_IN10
                                                | GPIO_Pin_1  // ADC12_IN11
                                                | GPIO_Pin_2; // ADC12_IN12
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AIN;  // ADC ÀÔ·ÂÀ¸·Î »ç¿ë
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
}

void TIM2_IRQHandler(void) {
  
  tim2_counter++; //카운트를 증가시킨다.
  
  if (led1) {
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
  } else {
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
  }
  led1 = !led1;
      
  if (tim2_counter % 5 == 0) {
    if (led2) {
      GPIO_SetBits(GPIOD, GPIO_Pin_3);
    } else {
      GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    }
    led2 = !led2;
  }
 
  printf("timer tick : %d\n", tim2_counter);
  
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void Delay(void) {
	int i;

	for (i = 0; i < 2000000; i++) {}
}

uint16_t getlength(uint16_t value) {
   uint16_t count = 0;
   while (value) {
     value /= 10;
     ++count;
   }
   return count;
}

int main(void)
{
    SystemInit();

    RCC_Configure();

    GPIO_Configure();

    ADC_Configure();

    NVIC_Configure();
    
    LCD_Init();
    Touch_Configuration();
   // Touch_Adjust();
    LCD_Clear(WHITE);
    
    ADC_start();
    int idx = 0;

    int dma_value = ADC_Value;
    while (1) {
        // TODO: implement 
        LCD_ShowString(10, 10, "Team 02", BLACK, WHITE);
        LCD_ShowNum(50, 50, pos_x, 5, BLACK, WHITE);
        LCD_ShowNum(50, 100, pos_y, 5, BLACK, WHITE);

        uint16_t new_pos_x = pos_x, new_pos_y = pos_y;
        Touch_GetXY(&new_pos_x, &new_pos_y, 1);
         
        if (new_pos_x != pos_x || new_pos_y != pos_y) {
            pos_x = new_pos_x;
            pos_y = new_pos_y;
            if (pos_x >= 100 && pos_x <= 2000 && pos_y >= 100 && pos_y <= 2000 ) {
              printf("clicked\n");
              LCD_Clear(WHITE);
            }
        }
        
    }
    return 0;
}
