#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "ILI93xx.h"
#include "key.h"
#include "malloc.h" 
#include "usmart.h" 
#include "GUI.h"
#include "touch.h"
#include "includes.h"

//ALIENTEK Mini STM32开发板范例代码27
//内存管理实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
extern void _MY_GetTouchPos(void);

/////////////////////////UCOSII任务设置///////////////////////////////////
//START 任务
#define START_TASK_PRIO      		20        //开始任务的优先级设置为最低
#define START_STK_SIZE  				64        //设置任务堆栈大小
OS_STK START_TASK_STK[START_STK_SIZE];    //任务堆栈	
void start_task(void *pdata);	            //任务函数

#define LED_TASK_PRIO      			9        //开始任务的优先级设置为最低
#define LED_STK_SIZE  					64        //设置任务堆栈大小
OS_STK LED_TASK_STK[LED_STK_SIZE];    //任务堆栈	
void led_task(void *pdata);	            //任务函数

#define EMWIN_DEMO_TASK_PRIO    8        //开始任务的优先级设置为最低
#define EMWIN_STK_SIZE  				3096        //设置任务堆栈大小
OS_STK EMWIN_TASK_STK[EMWIN_STK_SIZE];    //任务堆栈	
void emwin_demo_task(void *pdata);	            //任务函数

#define TOUCH_TASK_PRIO      		10        //开始任务的优先级设置为最低
#define TOUCH_STK_SIZE  				64        //设置任务堆栈大小
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];    //任务堆栈	
void touch_task(void *pdata);	            //任务函数

void BSP_Init(void)
{
	NVIC_Configuration();	 
	delay_init();	    			 //延时函数初始化	  
	uart_init(115200);	 		//串口初始化为9600
	LED_Init();		  				//初始化与LED连接的硬件接口
	TFTLCD_Init();			   	//初始化LCD		 
	tp_dev.init();
//	tp_dev.adjust();
	mem_init();				//初始化内存池

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
  GUI_Init();
	
}
void main_ui(void)
{
#if 0
	 _MY_GetTouchPos();
#endif
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("Hello World!", 30, 200);
	GUI_DispStringAt("Hello emWin!", 30, 216);
	GUI_DrawRoundedRect(0,0,200,200,5);
	GUI_DrawRoundedFrame(2,2,180,20,5,2);

}
void GUI_Text_Disp(void)
{
//字符大小6*8
	GUI_RECT rect = {0,48,80,64};
	GUI_DispChar('A');//显示单个字符
	GUI_DispCharAt('B',0,8);//在指定位置显示指定的单个字符
	GUI_DispChars('C',5);//指定字符显示的次数，从当前位置开始显示
	GUI_DispNextLine();//将光标移动至下一行
	GUI_DispChars('D',5);
	
	GUI_DispNextLine();
	GUI_DispString("dispstring");//在当前位置显示字符串
	GUI_DispStringAt("distring on pos 00000000000000000000000000000",0,32);//在指定位置开始显示字符串
	GUI_DispStringAtCEOL("dis and clear to end",72,32);//从指定位置开始显示字符串，并清除至行末
	GUI_DispStringHCenterAt("center display",80,40);//以指定位置为中点，左右延伸显示字符串，表现为居中形式,设定位置时需注意字符串长度
//	GUI_DispStringInRect("In rect disp1241235",&rect,GUI_TA_RIGHT);//
	GUI_DispStringInRectWrap("In rect disp1241235",&rect,GUI_TA_LEFT,GUI_WRAPMODE_CHAR);//在指定矩形区域显示字符，并设置对齐模式
																					//和自动换行模式
//	GUI_SetColor(GUI_RED);
//	GUI_FillRect(0,80,60,92);
	GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
	GUI_DispNextLine();
	GUI_DispString("dispstring");//在当前位置显示字符串
	
		GUI_SetTextMode(GUI_TEXTMODE_REV);// 显示反转文本，黑字白底
	GUI_DispNextLine();
	GUI_DispString("dispstring");//在当前位置显示字符
	
	GUI_SetTextAlign(GUI_TA_BOTTOM);
		GUI_SetTextMode(GUI_TEXTMODE_TRANS);//透明文本，屏幕上原有内容可见
	GUI_DispNextLine();
	GUI_DispString("dispstring");//在当前位置显示字符
	
		GUI_SetTextMode(GUI_TEXTMODE_XOR);//使用与底色相异的颜色显示字符
	GUI_DispNextLine();
	GUI_DispString("dispstring");//在当前位置显示字符
	
//	GUI_SetTextAlign(GUI_TA_BOTTOM);
//	GUI_DispNextLine();
//	GUI_DispString("text disp align");//在当前位置显示字符
	GUI_GotoXY(0,GUI_GetDispPosY());
	GUI_DispCEOL();//从当前文本位置清除至行末
	GUI_Clear();//清除整个显示窗口或屏幕
}
void emwin_texttest(void)
{
	int i;
	char acText[]  = "This example text wrapping";
	GUI_RECT Rect  ={0,200,70,320};  //???????? 

	GUI_WRAPMODE aWm[] = 
	{
		GUI_WRAPMODE_NONE,//无自动换行
		GUI_WRAPMODE_CHAR,//以字符模式换行
		GUI_WRAPMODE_WORD//以字模式换行
	};
	GUI_SetBkColor(GUI_BLUE);    //??????
	GUI_Clear();            //??
	GUI_SetFont(&GUI_Font24_ASCII);    //????
	GUI_SetColor(GUI_YELLOW);       //??????
	GUI_DispString("HELLO WORD!");
	GUI_SetFont(&GUI_Font8x16);    //????
	GUI_SetPenSize(10);        //?????
	GUI_SetColor(GUI_RED);      //????
	GUI_DrawLine(80,50,180,130);    //??
	GUI_DrawLine(80,130,180,50);    //??
	GUI_SetBkColor(GUI_BLACK);    //??????
	GUI_SetColor(GUI_WHITE);    //?????????
	GUI_SetTextMode(GUI_TM_NORMAL);  //????
	GUI_DispStringHCenterAt("GUI_TM_NORMAL",120,50);
	GUI_SetTextMode(GUI_TM_REV);    //????
	GUI_DispStringHCenterAt("GUI_TM_REV"    ,120,66);
	GUI_SetTextMode(GUI_TM_TRANS);  //????
	GUI_DispStringHCenterAt("GUI_TM_TRANS" ,120,82);
	GUI_SetTextMode(GUI_TM_XOR);      //????
	GUI_DispStringHCenterAt("GUI_TM_XOR"    ,120,98);
	GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_REV);//??????
	GUI_DispStringHCenterAt("GUI_EM_TRANS|GUI_TM_REV",120,114);
	GUI_SetTextMode(GUI_TM_TRANS);   //????
	for(i=0;i<3;i++)
	{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);

		GUI_DispStringInRectWrap(acText,&Rect,GUI_TA_LEFT,aWm[i]);
		Rect.x0 += 80;
		Rect.x1 += 80;
	} 
}
int main(void)
{
	BSP_Init();
//	main_ui();
	GUI_Text_Disp();
	emwin_texttest();
	OSInit();
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);//创建起始任务
	OSStart();
}
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr = 0;

//	GUI_Delay(1000);
	OS_ENTER_CRITICAL();
	OSTaskCreate(emwin_demo_task,(void *)0,&EMWIN_TASK_STK[EMWIN_STK_SIZE-1],EMWIN_DEMO_TASK_PRIO);
	OSTaskCreate(touch_task,(void *)0,&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],TOUCH_TASK_PRIO);
	OSTaskCreate(led_task,(void *)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();
}
void led_task(void *pdata)
{
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500);
	}
}
void touch_task(void *pdata)
{
	while(1)
	{
		GUI_TOUCH_Exec();
		OSTimeDlyHMSM(0,0,0,10);
	}
}
void emwin_demo_task(void *pdata)
{
	while(1)
	{
//		GUIDEMO_Main();
		OSTimeDlyHMSM(0,0,0,10);
	}
}
