#include "stm32f10x.h"
#include "string.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "math.h"
#include "parameter.h"
#include "led.h"
#include "pwm.h"
#include "key.h"
#include "glideLock.h"

void AnalyzePkg(void);
void MainWork(void);
void InitTestPin(void);
void printLogo(void);

u16 tick[4]={0,0,0,0};
u16 cpucnt;
u16 mainWorkRate;

u8 state;
u8 subState;
u8 keyState;
u16 ledInterval;
u8 ledFlash;

u8 caliCnt;
u8 caliNext;
float caliTmp[3];

float pwmOut[4];

s8 armed;
s8 dir;
s8 position;

int main(void)
{	
	u8 i,j,k;

	u8 t,t1;
	
	u8 ledr=1,ledg=1,ledb=0,ledt=0;
	
	u8 key,lastkey;
		
	delay_init();
	NVIC_Configuration();
	uart_init(115200);	
	LEDInit();	
	LEDSetRGB(ledr,ledg,ledb);
	LEDSet(1);
	KeyInit();
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	//printf("PPT FC\r\n");
	printLogo();
	//printf("cfrpg\r\n");
	printf("Loading parameters...\r\n");	
	ParamRead();
	if(params.headFlag!=0xCFCF||params.tailFlag!=0xFCFC)
	{
		ParamReset();
		ParamWrite();
		printf("Reset parameters.\r\n");		
	}	
	//params.ppm_enabled=1;
	MainClockInit();
	
	PWMInit(params.pwm_rate);
	//PWMInit(50);
	PWMInInit();
	//InitTestPin();
	delay_ms(50);
	ledInterval=5000;
	//ledFlash=1;
	
	GLInit();
	
	
//	mainWorkRate=10000/params.pwm_rate;
	mainWorkRate=10000/200;
	state=0;
	PWMLock(900);
	LEDSet(1);

//	if(params.pwm_rate<=100)
//		ledInterval=2500;
//	else if(params.pwm_rate<=250)
//		ledInterval=5000;
//	else
//		ledInterval=10000;
	ledr=1;
	ledg=0;
	ledb=1;	
	LEDSetRGB(ledr,ledg,ledb);
	while(1)
	{	
		//main work
		//200Hz
		if(tick[2]>=mainWorkRate)
		{
			tick[2]=0;
			//Select roll strength
			if(pwmValues[5]<1300)
			{
				GLState.rollValue=params.roll_step_val;
			}
			else if(pwmValues[5]<1600)
			{
				GLState.rollValue=params.roll_step_val<<1;
			}
			else
			{
				GLState.rollValue=params.roll_step_val*3;
			}
			if(state)
			{				
				if(pwmValues[0]<=950)
				{
					state=0;
					PWMSetVal(0,900);
					//ledFlash=0;
				}
				else
				{
					t=0;
					t1=0;
					if(pwmValues[3]>1500)
						t=1;
					if(pwmValues[4]>1500)
						t1=1;
					GLUpdate(t,t1);
					PWMSetVal(0,GLState.pwmBackup[0]);
					PWMSetVal(1,GLState.pwmBackup[1]);
					PWMSetVal(2,GLState.pwmBackup[2]);
				}
			}
			else
			{
				if(pwmValues[0]>950)
				{
					
					state=1;
					//LEDSet(1);
					//ledFlash=1;
				}
				PWMSetVal(0,900);
				PWMSetVal(1,(u16)pwmValues[1]);
				PWMSetVal(2,(u16)pwmValues[2]);
			}
			//printf("%d,%d,%d,%d,%d,%d\r\n",pwmValues[0],pwmValues[1],pwmValues[2],pwmValues[3],pwmValues[4],pwmValues[5]);
		}				
		//LED
		if(tick[0]>ledInterval)
		{			
			LEDFlip();
			tick[0]=0;
		}
		if(tick[1]>2000)
		{						
			tick[1]=0;	
			if(USART_RX_STA&0x8000)
			{				
				AnalyzePkg();
				USART_RX_STA=0;
			}
			key|=KeyRead();
			if((key&0x7)==3)
			{
				GLState.mode^=1;
				if(GLState.mode)
				{
					ledr=0;
					ledg=1;
					ledb=1;					
				}
				else
				{
					ledr=1;
					ledg=0;
					ledb=1;		
				}
				LEDSetRGB(ledr,ledg,ledb);
				LEDFlash(3);				
			}
			key<<=1;

//			for(i=0;i<6;i++)
//				printf("%d,",pwmValues[i]);
//			printf("\r\n");

		}		
	}
}



void AnalyzePkg(void)
{
	u8 len=USART_RX_STA&0x3FFF;
	u8 t,i;
	s32 v;
	
	if(USART_RX_BUF[0]=='s'&&USART_RX_BUF[1]=='e'&&USART_RX_BUF[2]=='t')
	{
		printf("SET CMD\r\n");
		if(len!=12)
		{
			printf("Invalid length:%d.\r\n",len);		
			return;
		}
		t=(USART_RX_BUF[3]-'0')*10+(USART_RX_BUF[4]-'0');
		printf("ID:%d\r\n",t);
		v=0;
		for(i=0;i<6;i++)
		{
			v*=10;
			v+=USART_RX_BUF[6+i]-'0';
		}
		if(USART_RX_BUF[5]=='-')
			v*=-1;
		printf("Value:%d\r\n",v);
		if(ParamSet(t,v)==0)
		{
			printf("Complete.\r\n");
			LEDFlash(5);
		}
		else
		{
			printf("Failed.\r\n");
			LEDFlash(3);
		}
		return;
	}
	if(USART_RX_BUF[0]=='s'&&USART_RX_BUF[1]=='h'&&USART_RX_BUF[2]=='o'&&USART_RX_BUF[3]=='w')
	{
		printf("SHOW CMD\r\n");
		ParamShow();
	}
	if(USART_RX_BUF[0]=='r'&&USART_RX_BUF[1]=='s'&&USART_RX_BUF[2]=='t'&&USART_RX_BUF[3]=='p')
	{
		printf("RSTP CMD\r\n");
		ParamReset();
		ParamWrite();
		printf("Param reset complete.\r\n");
		ParamShow();
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		u8 n=4;
		while(n--)
		{
			tick[n]++;
		}
		GLState.counter++;
		GLState.freqCounter++;
	}
}

void InitTestPin(void)
{
	GPIO_InitTypeDef gi;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_4;
	gi.GPIO_Mode=GPIO_Mode_Out_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gi);
}

void printLogo(void)
{
//printf(" _____  _____ _______ ______ _____ \r\n");
//printf("|  __ \\|  __ \\__   __|  ____/ ____|\r\n");
//printf("| |__) | |__) | | |  | |__ | |     \r\n");
//printf("|  ___/|  ___/  | |  |  __|| |     \r\n");
//printf("| |    | |      | |  | |   | |____ \r\n");
//printf("|_|    |_|      |_|  |_|    \\_____|\r\n");
	
printf("         _          _        _            _           _      \r\n");
printf("        /\\ \\       /\\ \\     /\\ \\         /\\ \\       /\\ \\     \r\n");
printf("       /  \\ \\     /  \\ \\    \\_\\ \\       /  \\ \\     /  \\ \\    \r\n");
printf("      / /\\ \\ \\   / /\\ \\ \\   /\\__ \\     / /\\ \\ \\   / /\\ \\ \\   \r\n");
printf("     / / /\\ \\_\\ / / /\\ \\_\\ / /_ \\ \\   / / /\\ \\_\\ / / /\\ \\ \\  \r\n");
printf("    / / /_/ / // / /_/ / // / /\\ \\ \\ / /_/_ \\/_// / /  \\ \\_\\ \r\n");
printf("   / / /__\\/ // / /__\\/ // / /  \\/_// /____/\\  / / /    \\/_/ \r\n");
printf("  / / /_____// / /_____// / /      / /\\____\\/ / / /          \r\n");
printf(" / / /      / / /      / / /      / / /      / / /________   \r\n");
printf("/ / /      / / /      /_/ /      / / /      / / /_________\\  \r\n");
printf("\\/_/       \\/_/       \\_\\/       \\/_/       \\/____________/ \r\n");
printf("                           by\r\n");
printf("               _____ ____ ___   ___   _____\r\n");
printf("              / ___// __// _ \\ / _ \\ / ___/\r\n");
printf("             / /__ / _/ / , _// ___// (_ / \r\n");
printf("             \\___//_/  /_/|_|/_/    \\___/ \r\n");
}