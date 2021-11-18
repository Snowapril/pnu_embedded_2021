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
void Delay(void);
void PWM_Configure(void);
void Timer_Configure(void);

int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};
    int led1 = 0, led2 = 0;
//---------------------------------------------------------------------------------------------------

uint16_t pos_x = 0;
uint16_t pos_y = 0;
uint16_t tim3_counter = 0;

void PWM_Configure(void) {
  uint16_t prescale = (uint16_t) (SystemCoreClock / 1000000) - 1;
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period = 9999; 
  TIM_TimeBaseStructure.TIM_Prescaler = 143;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1000; // us
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
   TIM_CtrlPWMOutputs(TIM2,ENABLE);  
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable); // TODO : OC1
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
}

void Timer_Configure(void) {
   NVIC_InitTypeDef NVIC_InitStructure;// 타이머 인터럽트 컨드롤
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;//타이머

    TIM_OCInitTypeDef OutputChannel;   
    
    /* TIM2 Clock Enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//클럭 할당
    
    /* Enable TIM2 Global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);//NVIC 초기화
    
    /* TIM2 Initialize */  
    TIM_TimeBaseStructure.TIM_Period=10000; // 100kHz// 주기
    TIM_TimeBaseStructure.TIM_Prescaler=7200; //
    TIM_TimeBaseStructure.TIM_ClockDivision=0;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//타임 초기화
    
    /* TIM2 PWM Initialize */
    OutputChannel.TIM_OCMode = TIM_OCMode_PWM1;
    OutputChannel.TIM_OutputState=TIM_OutputState_Enable;
    OutputChannel.TIM_OutputNState=TIM_OutputNState_Enable;
    OutputChannel.TIM_Pulse=50-1; // 50% duty ratio
    OutputChannel.TIM_OCPolarity=TIM_OCPolarity_Low;
    OutputChannel.TIM_OCNPolarity=TIM_OCNPolarity_High;
    OutputChannel.TIM_OCIdleState=TIM_OCIdleState_Set;
    OutputChannel.TIM_OCNIdleState=TIM_OCIdleState_Reset;
 
    TIM_OC1Init(TIM3,&OutputChannel);//오실리스코프 초기화
    
    /* TIM2 Enale */

}

void RCC_Configure(void) // stm32f10x_rcc.h ����
{
	/* ADC1 Clock enable */
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) // stm32f10x_gpio.h ����
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // TIM2_CH1 - PA0 
    // TIM2_CH2 - PA1 
    // TIM2_CH3 - PA2 O
    // TIM2_CH4 - PA3
    
  GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;  // ADC �Է����� ���
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;  // ADC �Է����� ���
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  // LED 1 PD2
  GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;  // ADC �Է����� ���
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  // LED 2 PD3
  GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;  // ADC �Է����� ���
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void NVIC_Configure() {
   /*TODO: NVIC_configuration */
   NVIC_InitTypeDef NVIC_InitStructure_ADC;

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

   NVIC_InitStructure_ADC.NVIC_IRQChannel = TIM2_IRQn;
   NVIC_InitStructure_ADC.NVIC_IRQChannelCmd = ENABLE;
   NVIC_InitStructure_ADC.NVIC_IRQChannelPreemptionPriority = 0x00;
   NVIC_InitStructure_ADC.NVIC_IRQChannelSubPriority = 0x00;
   NVIC_Init(&NVIC_InitStructure_ADC);

}

void timer_handle(int enable) {
  if (enable) {
    TIM_Cmd(TIM3,ENABLE); //타이머가 가득차면 인터럽트 발생
    TIM_ITConfig(TIM3,TIM_IT_Update | TIM_IT_CC1 ,ENABLE); // interrupt enable
  } else {
    TIM_Cmd(TIM3,DISABLE); //타이머가 가득차면 인터럽트 발생
    TIM_ITConfig(TIM3,TIM_IT_Update | TIM_IT_CC1 ,DISABLE); // interrupt enable
  }
  
}

void Delay(void) {
	int i;

	for (i = 0; i < 2000000; i++) {}
}

void TIM3_IRQHandler(void) {
    if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // Clear the interrupt flag
        tim3_counter++; //카운트를 증가시킨다.
        GPIOB->BRR = GPIO_Pin_0;  // PB0 OFF
        
        if (tim3_counter % 10 == 0) {
          if (led1) {
            GPIO_SetBits(GPIOD, GPIO_Pin_2);
          } else {
            GPIO_ResetBits(GPIOD, GPIO_Pin_2);
          }
          led1 = !led1;
        }
        
        if (tim3_counter % 50 == 0) {
          if (led2) {
            GPIO_SetBits(GPIOD, GPIO_Pin_3);
          } else {
            GPIO_ResetBits(GPIOD, GPIO_Pin_3);
          }
          led2 = !led2;
        }
        
        printf("timer tick : %d\n", tim3_counter);
    }

    if(TIM_GetITStatus(TIM3,TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);//인터럽트 플래그를 지운다.
        GPIOB->BSRR = GPIO_Pin_0;  // PB0 ON
    }
}

void make_pwn(u16 val)
{

    TIM_OCInitTypeDef OutputChannel; //오실리스코프 구조체 할당
    int pwm_pulse = val * 20000 / 100;
    OutputChannel.TIM_OCMode = TIM_OCMode_PWM1;
    OutputChannel.TIM_OutputState=TIM_OutputState_Enable;
    OutputChannel.TIM_OutputNState=TIM_OutputNState_Enable;
    OutputChannel.TIM_Pulse=pwm_pulse; // -90 1ms, 90 2ms
    OutputChannel.TIM_OCPolarity=TIM_OCPolarity_Low;
    OutputChannel.TIM_OCNPolarity=TIM_OCNPolarity_High;
    OutputChannel.TIM_OCIdleState=TIM_OCIdleState_Set;
    OutputChannel.TIM_OCNIdleState=TIM_OCIdleState_Reset;
   
  TIM_OC3Init(TIM2,&OutputChannel);// 구조체 할당
}

uint16_t getlength(uint16_t value) {
   uint16_t count = 0;
   while (value) {
     value /= 10;
     ++count;
   }
   return count;
}

void SG90_Init(float angle) {
  angle=(u16)(50.0*angle/9.0+249.0);
  TIM_SetCompare1(TIM2,angle);
}

int main(void)
{

    SystemInit();

    RCC_Configure();

    GPIO_Configure();

    NVIC_Configure();
    
    PWM_Configure();
    Timer_Configure();
    
    LCD_Init();
    Touch_Configuration();
   // Touch_Adjust();
    LCD_Clear(WHITE);
  
    int idx = 0;
    
    int btn_state = 0;
    
    while (1) {
    	// TODO: implement 
        LCD_ShowString(10, 10, "Team 02", BLACK, WHITE);
        LCD_ShowNum(50, 50, pos_x, 5, BLACK, WHITE);
        LCD_ShowNum(50, 100, pos_y, 5, BLACK, WHITE);

        if (btn_state) {
          LCD_DrawRectangle(100, 100, 200, 200);
          LCD_ShowString(110, 150, "OFF", BLACK, WHITE);
        } else {
          LCD_DrawRectangle(100, 100, 200, 200);
          LCD_ShowString(110, 150, "ON", BLACK, WHITE);
        }
        
        SG90_Init(idx++);
        make_pwn(idx++);
        
        uint16_t new_pos_x = pos_x, new_pos_y = pos_y;
        Touch_GetXY(&new_pos_x, &new_pos_y, 0);
         
        if (new_pos_x != pos_x || new_pos_y != pos_y) {
            pos_x = new_pos_x;
            pos_y = new_pos_y;
            printf("(%d, %d)\n", pos_x, pos_y);
            if (pos_x >= 100 && pos_x <= 2000 && pos_y >= 100 && pos_y <= 2000 ) {
              btn_state = !btn_state;
              timer_handle(btn_state);
            }
        }
        
        // TIM2->CCR1=10; 
        // LCD_Clear(color[idx++]);
        //idx = idx % (sizeof(color) / sizeof(int));
    }
    return 0;
}
