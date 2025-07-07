#include "main.h"
#include "key.h"
#include "delay.h"
#include "oled.h"
#include "bmp.h"
#include "usart.h"
#include "Encoder.h"    
#include "motor_ctrl.h"  
#include "gw_gray.h"
#include "Menu.h"
#include "PID.h"  
#include "Control.h"  

PID_TypeDef pid_motor1; 
PID_TypeDef pid_motor2; 

//期望速度
float Target_Speed = 0.0f;

int main(void)
{
	SYSCFG_DL_init(); 
	//delay_ms(100);
	BEEP_OFF();
	Menu_Init();
	NVIC_EnableIRQ_Init();
	PID_Init(&pid_motor1, 40.5f, 2.05f, 0.0f);
    PID_Init(&pid_motor2, 40.0f, 2.05f, 0.0f);
	while(1)
	{
		Menu_loop();
	}
}

void TIMER_0_INST_IRQHandler(void)
{
	static uint16_t tick_count_10ms = 0;// 10ms任务计数器
	static uint16_t tick_count_20ms = 0;// 20ms任务计数器
	static uint16_t tick_count_100ms = 0;// 10ms任务计数器
	static uint16_t tick_count_1000ms = 0;// 1000ms任务计数器

	// 10ms任务
	if (++tick_count_10ms >= 10) 
	{
		tick_count_10ms = 0;
		Key_Read(); // 读取按键状态
		Encoder_Speed(); // 计算编码器速度
		int pwm_output1 = (int)PID_realize(&pid_motor1, Target_Speed, Motor1_Speed);
        int pwm_output2 = (int)PID_realize(&pid_motor2, Target_Speed, Motor2_Speed);

        Set_Motor1_Speed(pwm_output1);
        Set_Motor2_Speed(pwm_output2);

		vofa_sendData(Target_Speed, Motor1_Speed, Motor2_Speed);
	}

	// 20ms任务
	if (++tick_count_20ms >= 100)
	{

		tick_count_20ms = 0;		
	}

	
	// 100ms任务
	if (++tick_count_100ms >= 100)
	{

		tick_count_100ms = 0;		
	}

	// 1000ms任务
	if (++tick_count_1000ms >= 1000)
	{
		tick_count_1000ms = 0;
		LED1_TOGGLE();
	}
	
}

void NVIC_EnableIRQ_Init(void)
{
    // 使能定时器0中断
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
	// 启动定时器
    DL_TimerG_startCounter(TIMER_0_INST);
	// 使能编码器中断
	NVIC_ClearPendingIRQ(Encoder_INT_IRQN);
    NVIC_EnableIRQ(Encoder_INT_IRQN);

}