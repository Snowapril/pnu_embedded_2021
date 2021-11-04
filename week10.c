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


void RCC_Configure(void) // stm32f10x_rcc.h 참고
{
	/* ADC1 Clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void ADC_Configure(void) {
  
  ADC_InitTypeDef ADC_InitStructure;
  
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
  ADC_InitStructure.ADC_NbrOfChannel = 3;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel10 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5); 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5); 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5); 
  /* Enable ADC1 DMA*/
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1*/
  ADC_Cmd(ADC1, ENABLE);
  
  /* Enable ADC1 reset calibaration register */  
  ADC_ResetCalibration(ADC1); 
  /* Check the end of ADC1 reset calibration register *
/* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1); 
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1)); 
  
  
  
  /* Start ADC1 Software Conversion */   
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);   
  

   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
   EXTI_InitTypeDef EXTI_InitStructure;
   EXTI_InitStructure.EXTI_Line = ADC_IT_EOC;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);
  
}

void GPIO_Configure(void) // stm32f10x_gpio.h 참고
{
    GPIO_InitTypeDef GPIO_InitStructure;

	// TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
	/* Configure ADC_Input pins */
  GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0      // ADC12_IN10
                                                | GPIO_Pin_1  // ADC12_IN11
                                                | GPIO_Pin_2; // ADC12_IN12
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AIN;  // ADC 입력으로 사용
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
}


void NVIC_Configure() {
   /*TODO: NVIC_configuration */
   NVIC_InitTypeDef NVIC_InitStructure_ADC;

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

   NVIC_InitStructure_ADC.NVIC_IRQChannel = ADC1_2_IRQn;
   NVIC_InitStructure_ADC.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure_ADC.NVIC_IRQChannelPreemptionPriority = 0x00;
   NVIC_InitStructure_ADC.NVIC_IRQChannelSubPriority = 0x00;
   NVIC_Init(&NVIC_InitStructure_ADC);

}

void ADC1_2_IRQHandler(void) {
    ADC_GetITStatus(ADC1, ADC_IT_EOC);
    value = ADC_GetConversionValue(ADC1);
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}

void EXTI15_10_IRQHandler(void) { // when the button is pressed

	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		/* send UART 1 */
        EXTI_ClearITPendingBit(EXTI_Line11);
	}
}


void Delay(void) {
	int i;

	for (i = 0; i < 2000000; i++) {}
}

void sendDataUART1(uint16_t data) {
	USART_SendData(USART1, data);
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
  
    int idx = 0;
    while (1) {
    	// TODO: implement 
        LCD_ShowString(10, 10, "Team 02", BLACK, WHITE);
        LCD_ShowNum(50, 50, pos_x, 5, BLACK, WHITE);
        LCD_ShowNum(50, 100, pos_y, 5, BLACK, WHITE);
        
        Touch_GetXY(&pos_x, &pos_y, 1);
         
        //LCD_Clear(color[idx++]);
       //idx = idx % (sizeof(color) / sizeof(int));
    }
    return 0;
}
