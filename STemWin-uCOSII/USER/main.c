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

//ALIENTEK Mini STM32�����巶������27
//�ڴ����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
extern void _MY_GetTouchPos(void);

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
#define START_TASK_PRIO      		20        //��ʼ��������ȼ�����Ϊ���
#define START_STK_SIZE  				64        //���������ջ��С
OS_STK START_TASK_STK[START_STK_SIZE];    //�����ջ	
void start_task(void *pdata);	            //������

#define LED_TASK_PRIO      			9        //��ʼ��������ȼ�����Ϊ���
#define LED_STK_SIZE  					64        //���������ջ��С
OS_STK LED_TASK_STK[LED_STK_SIZE];    //�����ջ	
void led_task(void *pdata);	            //������

#define EMWIN_DEMO_TASK_PRIO    8        //��ʼ��������ȼ�����Ϊ���
#define EMWIN_STK_SIZE  				3096        //���������ջ��С
OS_STK EMWIN_TASK_STK[EMWIN_STK_SIZE];    //�����ջ	
void emwin_demo_task(void *pdata);	            //������

#define TOUCH_TASK_PRIO      		10        //��ʼ��������ȼ�����Ϊ���
#define TOUCH_STK_SIZE  				64        //���������ջ��С
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];    //�����ջ	
void touch_task(void *pdata);	            //������

void BSP_Init(void)
{
	NVIC_Configuration();	 
	delay_init();	    			 //��ʱ������ʼ��	  
	uart_init(115200);	 		//���ڳ�ʼ��Ϊ9600
	LED_Init();		  				//��ʼ����LED���ӵ�Ӳ���ӿ�
	TFTLCD_Init();			   	//��ʼ��LCD		 
	tp_dev.init();
//	tp_dev.adjust();
	mem_init();				//��ʼ���ڴ��

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
//�ַ���С6*8
	GUI_RECT rect = {0,48,80,64};
	GUI_DispChar('A');//��ʾ�����ַ�
	GUI_DispCharAt('B',0,8);//��ָ��λ����ʾָ���ĵ����ַ�
	GUI_DispChars('C',5);//ָ���ַ���ʾ�Ĵ������ӵ�ǰλ�ÿ�ʼ��ʾ
	GUI_DispNextLine();//������ƶ�����һ��
	GUI_DispChars('D',5);
	
	GUI_DispNextLine();
	GUI_DispString("dispstring");//�ڵ�ǰλ����ʾ�ַ���
	GUI_DispStringAt("distring on pos 00000000000000000000000000000",0,32);//��ָ��λ�ÿ�ʼ��ʾ�ַ���
	GUI_DispStringAtCEOL("dis and clear to end",72,32);//��ָ��λ�ÿ�ʼ��ʾ�ַ��������������ĩ
	GUI_DispStringHCenterAt("center display",80,40);//��ָ��λ��Ϊ�е㣬����������ʾ�ַ���������Ϊ������ʽ,�趨λ��ʱ��ע���ַ�������
//	GUI_DispStringInRect("In rect disp1241235",&rect,GUI_TA_RIGHT);//
	GUI_DispStringInRectWrap("In rect disp1241235",&rect,GUI_TA_LEFT,GUI_WRAPMODE_CHAR);//��ָ������������ʾ�ַ��������ö���ģʽ
																					//���Զ�����ģʽ
//	GUI_SetColor(GUI_RED);
//	GUI_FillRect(0,80,60,92);
	GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
	GUI_DispNextLine();
	GUI_DispString("dispstring");//�ڵ�ǰλ����ʾ�ַ���
	
		GUI_SetTextMode(GUI_TEXTMODE_REV);// ��ʾ��ת�ı������ְ׵�
	GUI_DispNextLine();
	GUI_DispString("dispstring");//�ڵ�ǰλ����ʾ�ַ�
	
	GUI_SetTextAlign(GUI_TA_BOTTOM);
		GUI_SetTextMode(GUI_TEXTMODE_TRANS);//͸���ı�����Ļ��ԭ�����ݿɼ�
	GUI_DispNextLine();
	GUI_DispString("dispstring");//�ڵ�ǰλ����ʾ�ַ�
	
		GUI_SetTextMode(GUI_TEXTMODE_XOR);//ʹ�����ɫ�������ɫ��ʾ�ַ�
	GUI_DispNextLine();
	GUI_DispString("dispstring");//�ڵ�ǰλ����ʾ�ַ�
	
//	GUI_SetTextAlign(GUI_TA_BOTTOM);
//	GUI_DispNextLine();
//	GUI_DispString("text disp align");//�ڵ�ǰλ����ʾ�ַ�
	GUI_GotoXY(0,GUI_GetDispPosY());
	GUI_DispCEOL();//�ӵ�ǰ�ı�λ���������ĩ
	GUI_Clear();//���������ʾ���ڻ���Ļ
}
void emwin_texttest(void)
{
	int i;
	char acText[]  = "This example text wrapping";
	GUI_RECT Rect  ={0,200,70,320};  //???????? 

	GUI_WRAPMODE aWm[] = 
	{
		GUI_WRAPMODE_NONE,//���Զ�����
		GUI_WRAPMODE_CHAR,//���ַ�ģʽ����
		GUI_WRAPMODE_WORD//����ģʽ����
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
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);//������ʼ����
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
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
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
