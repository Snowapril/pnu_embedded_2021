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
uint16_t tim2_counter = 0;
int btn_state = 0;

  
void PWM_Configure(void) {
  uint16_t prescale = (uint16_t) (SystemCoreClock / 1000000) - 1;
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period = 20000-1; 
  TIM_TimeBaseStructure.TIM_Prescaler = prescale;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1500; // us
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Disable); // TODO : OC1
  
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
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

void RCC_Configure(void) // stm32f10x_rcc.h ����
{
	/* ADC1 Clock enable */
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) // stm32f10x_gpio.h ����
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // TIM4 Init(서보모터)
  GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
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

void TIM2_IRQHandler(void) {
  
  if (btn_state) {
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
  }
 
  printf("timer tick : %d\n", tim2_counter);
  
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void make_pwm(int degree)
{
    int pwm_pulse = ((2300 - 700) / 180) * (degree - 15) + 700;
    TIM_OCInitTypeDef tim_oc_init_struct;
    tim_oc_init_struct.TIM_OCMode = TIM_OCMode_PWM1;
    tim_oc_init_struct.TIM_OutputState=TIM_OutputState_Enable;
    tim_oc_init_struct.TIM_Pulse=pwm_pulse; // -90 1ms, 90 2ms
    tim_oc_init_struct.TIM_OCPolarity=TIM_OCPolarity_High;
    printf("pulse : %d\n", pwm_pulse);
    TIM_OC3Init(TIM4,&tim_oc_init_struct);// 구조체 할당
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

    PWM_Configure();
    Timer_Configure();
    
    LCD_Init();
    Touch_Configuration();
   // Touch_Adjust();
    LCD_Clear(WHITE);
      
    
    while (1) {
    	// TODO: implement 
        LCD_ShowString(10, 10, "Team 02", BLACK, WHITE);
        LCD_ShowNum(50, 50, pos_x, 5, BLACK, WHITE);
        LCD_ShowNum(50, 100, pos_y, 5, BLACK, WHITE);

        if (btn_state) {
          LCD_DrawRectangle(100, 100, 200, 200);
          LCD_ShowString(110, 150, "ON", BLACK, WHITE);
        } else {
          LCD_DrawRectangle(100, 100, 200, 200);
          LCD_ShowString(110, 150, "OFF", BLACK, WHITE);
        }
        
        uint16_t new_pos_x = pos_x, new_pos_y = pos_y;
        Touch_GetXY(&new_pos_x, &new_pos_y, 1);
         
        if (new_pos_x != pos_x || new_pos_y != pos_y) {
            pos_x = new_pos_x;
            pos_y = new_pos_y;
            if (pos_x >= 100 && pos_x <= 2000 && pos_y >= 100 && pos_y <= 2000 ) {
              btn_state = !btn_state;
              printf("clicked\n");
              timer_handle(btn_state);
              LCD_Clear(WHITE);
            }
        }
        
    }
    return 0;
}
