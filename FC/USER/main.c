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
#include "adc.h"
#include "pid.h"
#include "rtc.h"

void Init(void);
void UpdateFlight(void);
void UpdateCali(void);
void UpdateESCCali(void);
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
PIDData phiPID;
s8 armed;
s8 dir;
s8 position;

int main(void)
{	
	u8 i,j,k;

	u8 t;
	
	u16 ct;
	
	//s32 tlon=0;
	delay_init();
	NVIC_Configuration();
	uart_init(115200);
	
	LEDInit();
	PWMInit();
	KeyInit();
	ADCInit();
	RTCInit();
	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	printf("PPT FC\r\n");
	printf("Read parameters.\r\n");
	
	ParamRead();
	if(params.headFlag!=0xCFCF||params.tailFlag!=0xFCFC)
	{
		ParamReset();
		ParamWrite();
		printf("Reset parameters.\r\n");		
	}
	if(params.phi_k<0.001&&params.phi_k>-0.001)
	{
		printf("RP uncalibrate.\r\n");
	}
	else
	{
		printf("RP calibrated.\r\n");
		printf("k=%f\tb=%f\r\n",params.phi_k,params.phi_b);
	}
	MainClockInit();
	
	Init();

	while(1)
	{		
		if(tick[3]>=10)
		{
			float phi=ADCReadVol();
			phi=params.phi_k*phi+params.phi_b;
			float dt=tick[3]/1000.0;
			tick[3]=0;
			PIDCalc(&phiPID,-phi,dt);			
		}
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
			
			tick[1]=0;	
			if(USART_RX_STA&0x8000)
			{
				//printf("receive %d\r\n",USART_RX_STA&0x3FFF);
				AnalyzePkg();
				USART_RX_STA=0;
			}
			
			
			//printf("%f %d\r\n",pwmOut[1],pwmState[1].value);
		}
		//main work
		if(tick[2]>=20)
		{
			tick[2]=0;	
			keyState<<=1;
			keyState|=KEY;
			keyState&=0x07;
			switch(state)
			{
				case 0:UpdateFlight();break;
				case 1:UpdateCali();break;
				case 2:UpdateESCCali();break;
			}
		}		
	}
}

void Init(void)
{
	u8 i;
	keyState=0;
	state=0;
	ledInterval=1000;
	ledFlash=0;
	
	for(i=0;i<3;i++)
	{
		pwmOut[i]=0;
	}
	pwmOut[3]=-1;
	
	phiPID.P=params.phi_p;
	phiPID.I=params.phi_i;
	phiPID.D=params.phi_d;
	phiPID.lpf=0;
	phiPID.maxint=100;
	phiPID.maxout=1;
	phiPID.minout=-1;
	phiPID.lasterr=0;
	phiPID.tau=0.01;
	phiPID.err=0;
	phiPID.integral=0;
	phiPID.derivate=0;
	
	armed=0;
	
	if(SW1)
		dir=1;
	else
		dir=-1;
	if(SW2)
		position=0;
	else
		position=1;
}

void UpdateFlight(void)
{
	float left,right;
	float aile=0,elev=0,rudd=0,thro=-1;
	if(pwmState[3].value<1000)
		armed--;
	else
		armed=10;
	if(armed<0)
		armed=0;
	if(pwmState[0].value!=0)
		aile=(pwmState[0].value-1500)/500.0;	
	if(pwmState[1].value!=0)
		elev=(pwmState[1].value-1500)/500.0;
	if(pwmState[2].value!=0)
		rudd=(pwmState[2].value-1500)/500.0;
	if(pwmState[3].value!=0)
		thro=(pwmState[3].value-1000)/1000.0;
	
	right=elev*params.elev+dir*aile*params.aile-phiPID.out*params.phi;
	if(right>1)
		right=1;
	if(right<-1)
		right=-1;
	left=-right;

	if(thro>0)
	{
		if(position)
			thro=thro+rudd*params.yaw_p*params.yaw_scale;
		else
			thro=thro+rudd*params.yaw_p;
	}
	
	pwmOut[0]=(left+1)/2;
	pwmOut[1]=(right+1)/2;
	pwmOut[2]=0.5;
	pwmOut[3]=thro;
	
	PWMSet(pwmOut);
	
	if(keyState==0x04)
	{
		if(SW3==0&&SW4==0)
		{
			state=1;
			ledInterval=500;
			subState=0;
			caliCnt=0;
			caliNext=0;
			caliTmp[0]=0;
			caliTmp[1]=0;
			caliTmp[2]=0;
		}
		if(SW3==1&&SW4==0)
		{
			state=2;
			subState=0;
			printf("Start ESC Cali.\r\n");
		}
		if(SW3==1&&SW4==1)
		{
			if(SW1)
				dir=1;
			else
				dir=-1;
			if(SW2)
				position=0;
			else
				position=1;
			LEDFlash(2);
		}
	}
}

void UpdateCali(void)
{	
	float sum=0,tf;
	if(subState==0)
	{
		ledFlash=caliNext+1;
		if(keyState==0x04)
		{
			subState=1;
			ledFlash=0;
			ledInterval=100;
			caliCnt=0;
			caliTmp[caliNext]=0;
		}
	}
	else if(subState==1)
	{
		if(caliCnt>=100)
		{
			caliTmp[caliNext]/=caliCnt;
			caliNext++;
			if(caliNext<3)
			{
				subState=0;
				ledInterval=500;
			}
			else
			{
				LEDSet(0);
				
				sum=caliTmp[0]+caliTmp[1]+caliTmp[2];
				tf=(caliTmp[0]*caliTmp[0]+caliTmp[1]*caliTmp[1]+caliTmp[2]*caliTmp[2])-sum*sum/3;
				params.phi_k=0.5235988*(caliTmp[0]-caliTmp[2])/tf;
				params.phi_b=-params.phi_k*sum/3;
				ParamWrite();
				
				state=0;
				ledInterval=1000;
				ledFlash=0;
				printf("Cali complete:\r\n");
				printf("Vol:%f %f %f\r\n",caliTmp[0],caliTmp[1],caliTmp[2]);
				printf("k=%f\tb=%f\r\n",params.phi_k,params.phi_b);
			}
		}
		caliTmp[caliNext]+=ADCReadVol();
		caliCnt++;
	}

}

void UpdateESCCali(void)
{
	if(subState==0)
	{
		ledInterval=1000;
		ledFlash=1;
		pwmOut[0]=1;
		pwmOut[1]=1;
		pwmOut[2]=1;
		pwmOut[3]=1;
		PWMSet(pwmOut);
		if(keyState==0x04)
		{
			subState=1;
			printf("ESC Cali 2.\r\n");
		}
	}
	else if(subState==1)
	{
		ledInterval=1000;
		ledFlash=2;
		pwmOut[0]=0;
		pwmOut[1]=0;
		pwmOut[2]=0;
		pwmOut[3]=0;
		PWMSet(pwmOut);
		if(keyState==0x04)
		{
			subState=0;
			state=0;
			ledInterval=1000;
			ledFlash=0;
			printf("ESC Cali complete.\r\n");
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
