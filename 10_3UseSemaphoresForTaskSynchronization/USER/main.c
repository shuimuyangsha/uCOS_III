#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "includes.h"
#include "math.h"
//ALIENTEK 探索者STM32F407开发板 UCOSIII实验
//例10-3 UCOSIII 信号量用于任务同步

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define TASK1_TASK_PRIO		5
//任务堆栈大小	
#define TASK1_STK_SIZE 		128
//任务控制块
OS_TCB Task1_TaskTCB;
//任务堆栈	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);


//任务优先级
#define TASK2_TASK_PRIO		4
//任务堆栈大小	
#define TASK2_STK_SIZE 		128
//任务控制块
OS_TCB Task2_TaskTCB;
//任务堆栈	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
void task2_task(void *p_arg);

//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

OS_SEM	SYNC_SEM;		//定义一个信号量，用于任务同步

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);  //时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	uart_init(115200);   //串口初始化
	LED_Init();         //LED初始化	
	LCD_Init();			//LCD初始化	
	KEY_Init();			//按键初始化
	StandbyIO_Init();
	
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 10-3");
	LCD_ShowString(30,50,200,16,16,"Sem Sync");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2014/12/5");
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,234,314);	
	LCD_DrawLine(5,130,234,130);
	POINT_COLOR = RED;
	LCD_ShowString(30,111,200,16,16,"SYNC_SEM Value:  0");
	POINT_COLOR = BLUE;
	
	OSInit(&err);		    	//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区			 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}


//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
		
	OS_CRITICAL_ENTER();	//进入临界区
	//创建一个信号量
	OSSemCreate ((OS_SEM*	)&SYNC_SEM,
                 (CPU_CHAR*	)"SYNC_SEM",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//创建TASK1任务
	OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )task1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK1_TASK_PRIO,     
                 (CPU_STK   * )&TASK1_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			
	//创建TASK2任务
	OSTaskCreate((OS_TCB 	* )&Task2_TaskTCB,		
				 (CPU_CHAR	* )"Task2 task", 		
                 (OS_TASK_PTR )task2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK2_TASK_PRIO,     
                 (CPU_STK   * )&TASK2_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				 
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}


u8 Debug_task1_num;
CPU_TS Debug_task1_CyclesStart, Debug_task1_CyclesDelta;
CPU_TS Debug_task1_ts;

OS_SEM_CTR Debug_SemaphoreCount = 0;
float x = 1.0;
float y = 1.0;
//任务1的任务函数
void task1_task(void *p_arg)
{
	u8 key;
	OS_ERR err;
	while(1)
	{
		StandbyIO1(1);
		y = tanh(x);
		Debug_task1_CyclesStart = OS_TS_GET();

		key = KEY_Scan(0);  //扫描按键
		if(key==WKUP_PRES)	
		{
			OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量
			LCD_ShowxNum(150,111,SYNC_SEM.Ctr,3,16,0);	//显示信号量值
		}

		if (key == KEY0_PRES)
		{			
			OSSemPendAbort(&SYNC_SEM, OS_OPT_PEND_ABORT_1, &err);
			OSSemSet(&SYNC_SEM, Debug_SemaphoreCount, &err);
			LCD_ShowxNum(150, 111, SYNC_SEM.Ctr, 3, 16, 0);	//显示信号量值
		}

		if (key == KEY1_PRES)
		{
			OSSemDel(&SYNC_SEM, OS_OPT_DEL_ALWAYS, &err);
			
		}


		StandbyIO1(0);
		Debug_task1_ts = OS_TS_GET() - Debug_task1_CyclesStart;

		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);   //延时10ms

		Debug_task1_CyclesDelta = OS_TS_GET() - Debug_task1_CyclesStart;
	}
}


u8 Debug_task2_num;
CPU_TS Debug_task2_CyclesStart, Debug_task2_CyclesDelta;
CPU_TS Debug_task2_ts;
CPU_TS Debug_Sem_ts;
CPU_INT16U Debug_DelayTimeSet[4] = {0,0,1,0};//时、分、秒、毫秒


//任务2的任务函数
void task2_task(void *p_arg)
{	
	u8 num;
	OS_ERR err;
	while(1)
	{
		Debug_task2_CyclesStart = OS_TS_GET();
		OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING, &Debug_Sem_ts,&err); //请求信号量



		StandbyIO2(1);
		
		num++;
		LCD_ShowxNum(150,111,SYNC_SEM.Ctr,3,16,0);			//显示信号量值
		LCD_Fill(6,131,233,313,lcd_discolor[num%14]);		//刷屏
		LED1 = ~LED1;
		DebugLED1.DebugGetLED = GPIO_ReadOutputDataBit(GPIOF, GPIO_Pin_10);

		Debug_task2_ts = OS_TS_GET() - Debug_task2_CyclesStart;

		StandbyIO2(0);
		OSTimeDlyHMSM(0,0, Debug_DelayTimeSet[2], Debug_DelayTimeSet[3],OS_OPT_TIME_PERIODIC,&err);   //延时1s

		Debug_task2_CyclesDelta = OS_TS_GET() - Debug_task2_CyclesStart;
	}
}





	


