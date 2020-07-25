#include "stm32f10x.h"
#include "string.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "math.h"
#include "parameter.h"
#include "led.h"
#include "gps.h"
#include "mb85rs.h"
#include "ms4525do.h"
#include "i2c.h"
#include "pwm.h"
#include "sdp3x.h"
#include "tle493d.h"

void AnalyzePkg(void);

u16 tick[4]={0,0,0,0};
u16 cpucnt;

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

	u8 t;
	
	u16 ct;
	
	u16 tmp16[4];
	s16 stmp16[3];
	u32 cnt=0;
	//s32 tlon=0;
	delay_init();
	NVIC_Configuration();
	uart_init(115200);
	
	LEDInit();
	//GPSInit(9600);
	//FRAMInit();
	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	printf("PPT FC\r\n");
	printf("cfrpg\r\n");
	printf("Read parameters.\r\n");
	
	ParamRead();
	if(params.headFlag!=0xCFCF||params.tailFlag!=0xFCFC)
	{
		ParamReset();
		ParamWrite();
		printf("Reset parameters.\r\n");		
	}
	MainClockInit();
	PWMInInit();
	printf("Init I2C\r\n");
	I2CInit();
	printf("Init Sensors\r\n");
//	MS4525DOInit(1);
//	MS4525DOInit(2);
//	MS4525DOInit(3);
	delay_ms(50);
	Sdp3xInit(1);
	Sdp3xSetMode(1,SDP3X_ADDR1,0x3603);
	delay_ms(20);
	ledInterval=500;
//	printf("Clear Console in 5s.\r\n");
//	for(i=0;i<10;i++)
//	{
//		delay_ms(500);
//	}
//	printf("t,state,temp,dp1,dp2,dp3,\r\n");
	TLE493DInit(2,TLE493D_ADDR1);
	while(1)
	{			
		//LED
		if(tick[0]>=ledInterval)
		{				
			if(ledFlash)
			{
				LEDFlash(ledFlash);
				LEDSet(0);
			}
			else
			{
				LEDFlip();
			}
			tick[0]=0;
		}
		if(tick[1]>=500)
		{
			//printf("%d,",tick[1]);
			tick[2]=0;
			tick[1]=0;	
			if(USART_RX_STA&0x8000)
			{
				//printf("receive %d\r\n",USART_RX_STA&0x3FFF);
				AnalyzePkg();
				USART_RX_STA=0;
			}
			//printf("%d,",cnt++);
//			if(pwmState[0].value>1500)
//				printf("1,");
//			else
//				printf("0,");
//			MS4525DOGetRawData(1,tmp16);
//			printf("%d,%d,",tmp16[1],tmp16[0]);
			//MS4525DOGetRawData(2,tmp16);
			//printf("%d,",tmp16[0]);
//			MS4525DOGetRawData(3,tmp16);
//			printf("%d,\r\n",tmp16[0]);
			//Sdp3xReadOut(1,SDP3X_ADDR1,2,stmp16);
			//printf("%d,%d\r\n",stmp16[0],stmp16[1]);
			//printf("%d\r\n",tick[2]);
			//printf("%f %d\r\n",pwmOut[1],pwmState[1].value);
			TLE493DReadout(2,TLE493D_ADDR1,4,tmp16);
		}
		//main work
//		if(tick[2]>=20)
//		{
//			if(gpsReady)
//			{
//				//printf("%s",gpsBuff[gpsCurrBuff]);
//				gpsReady=0;
//			}
//		}		
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

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 
		u8 n=4;
		while(n--)
		{
			tick[n]++;
		}
	}
}
