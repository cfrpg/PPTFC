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

#include "math.h"

#define TwoPI 6.283185
#define MaxThro 1.0

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
PIDData pwrPID;
float ctrlTarget;
s8 armed;

u8 CurrCtrl;
u8 waveEnabled;

float volt,curr;
float dthro;
float outThro;

float waveValue;
float waveT;
float wavePeroid;

int main(void)
{	
	u8 i,j,k;

	u8 t;
	
	u16 ct;
	
	float temp;
	
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
	if(params.pwm_max-params.pwm_min<500)
	{
		printf("RC uncalibrate.\r\n");
	}
	else
	{
		printf("RC calibrated.\r\n");
		printf("min=%d\tmax=%d\r\n",params.pwm_min,params.pwm_max);
	}
	ParamShow();
	MainClockInit();
	
	Init();

	while(1)
	{		
		if(tick[3]>=10)
		{			
			
			volt=ADCReadVol(1)*params.volt_gain;
			curr=ADCReadVol(0)*params.curr_gain;
			if(armed>0)
			{	
				float dt=tick[3]/1000.0;
				if(waveEnabled)
				{
					waveValue=(1+sin(TwoPI*params.freq*waveT)*params.strength);
					temp=ctrlTarget*waveValue;
					
					waveT+=dt;
					if(waveT>wavePeroid)
					{
						waveT-=wavePeroid;
					}
				}
				else
				{
					temp=ctrlTarget;
				}
				
				if(CurrCtrl)
				{
					PIDCalc(&pwrPID,temp-curr,dt);
				}
				else
				{
					PIDCalc(&pwrPID,temp-curr*volt,dt);
				}
				dthro=pwrPID.out;
				outThro+=dthro*dt;
				if(outThro>MaxThro)
					outThro=MaxThro;
				if(outThro<0)
				{
					outThro=0;
					if(dthro<0)
						dthro=0;
				}
			}
//			float phi=ADCReadVol();
//			phi=params.phi_k*phi+params.phi_b;
//			
			tick[3]=0;
//			PIDCalc(&phiPID,-phi,dt);		
				
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
//			if(position)
//				printf("%f\r\n",ADCReadVol());
				
			//printf("%f %d\r\n",pwmOut[1],pwmState[1].value);
			//printf("%f\t%f\t%f\t%f\r\n",curr*volt,ctrlTarget,pwrPID.out,outThro);
		}
		//main work
		if(tick[2]>=20)
		{
			//printf("state:%d\r\n",state);
			
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

void clearPID()
{
	pwrPID.lasterr=0;	
	pwrPID.err=0;
	pwrPID.integral=0;
	pwrPID.derivate=0;
	dthro=0;
	outThro=0;
	waveT=0;
	waveValue=0;
	if(params.freq>0.01)		
		wavePeroid=1/params.freq;
	else
		wavePeroid=100000;
}

void readSW12(void)
{
	if(SW1)
		CurrCtrl=0;
	else
		CurrCtrl=1;
	if(SW2)
		waveEnabled=0;
	else
		waveEnabled=1;
	if(CurrCtrl)
	{
		volt=ADCReadVol(1)*params.volt_gain;
		pwrPID.P=params.pwr_p/volt;
		pwrPID.I=params.pwr_i/volt;
		pwrPID.D=params.pwr_d/volt;
		
	}
	else
	{
		pwrPID.P=params.pwr_p;
		pwrPID.I=params.pwr_i;
		pwrPID.D=params.pwr_d;
	}
	pwrPID.maxint=params.pwr_i_max;	
	clearPID();
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
	
	pwrPID.P=params.pwr_p;
	pwrPID.I=params.pwr_i;
	pwrPID.D=params.pwr_d;
	pwrPID.lpf=0.5;
	pwrPID.maxint=params.pwr_i_max;
	pwrPID.maxout=10;
	pwrPID.minout=-10;
	pwrPID.lasterr=0;
	pwrPID.tau=0.01;
	pwrPID.err=0;
	pwrPID.integral=0;
	pwrPID.derivate=0;
	
	armed=-1;
	
	volt=0;
	curr=0;
	
	readSW12();
}

void UpdateFlight(void)
{
	float left,right;
	float aile=0,elev=0,rudd=0,thro=-1;
	float out;
//	if(pwmState[3].value<1000)
//		armed--;
//	else
//		armed=10;
	
		
//	if(pwmState[0].value!=0)
//		aile=(pwmState[0].value-1500)/500.0;	
//	if(pwmState[1].value!=0)
//		elev=(pwmState[1].value-1500)/500.0;
//	if(pwmState[2].value!=0)
//		rudd=(pwmState[2].value-1500)/500.0;
	if(pwmState[3].value!=0)
		thro=(float)(pwmState[3].value-params.pwm_min)/(params.pwm_max-params.pwm_min);
	if(armed>=0&&thro>0.1)
	{
		armed=1;
		if(CurrCtrl)
		{
			ctrlTarget=(thro-0.1)/0.9*params.curr_max;
		}
		else
		{
			ctrlTarget=(thro-0.1)/0.9*params.power_max;
		}
	}
	
	//out=pwrPID.out;
	out=outThro;
	//out=thro*waveValue;
	//printf("pwm:%d,thro:%f,tgt:%f\r\n",pwmState[3].value,thro,ctrlTarget);
	//printf("%f\t%f\t%f\r\n",volt,curr,out);
	if(keyState==0x04)
	{
		if(SW3==0&&SW4==0)
		{
			//PWM IN CALI
			pwmOut[0]=0;
			pwmOut[1]=0;
			pwmOut[2]=0;
			pwmOut[3]=0;
			armed=0;
			
			state=1;
			ledInterval=500;
			subState=0;
			caliCnt=0;
			caliNext=0;
			caliTmp[0]=0;
			caliTmp[1]=0;
			caliTmp[2]=0;
			printf("Start RC Cali.\r\n");
		}
		if(SW3==1&&SW4==0)
		{
			state=2;
			subState=0;
			printf("Start ESC Cali.\r\n");
		}
		if(SW3==1&&SW4==1)
		{
			readSW12();
			printf("Status\r\n");
			printf("volt:%f\tcurr:%f\r\n",volt,curr);
			if(CurrCtrl)
				printf("Current control.\r\n");
			else
				printf("Power control.\r\n");
			if(waveEnabled)
				printf("Wave enabled.\r\n");
			else
				printf("Wave disabled.\r\n");
			LEDFlash(2);
		}
	}
	if(armed<0)
	{
		//Start up
		if(thro<0.05)
			armed=0;
	}
	if(armed>0)
	{
		if(thro<0.1)
		{
			armed=0;
			clearPID();
		}
	}
	
	//	pwmOut[0]=(left+1)/2;
//	pwmOut[1]=(right+1)/2;
//	pwmOut[2]=0.5;
//	pwmOut[3]=thro;
	pwmOut[0]=out;
	pwmOut[1]=out;
	pwmOut[2]=out;
	pwmOut[3]=out;
	if(armed<=0)
	{
		ctrlTarget=0;
		pwmOut[0]=0;
		pwmOut[1]=0;
		pwmOut[2]=0;
		pwmOut[3]=0;
	}
	PWMSet(pwmOut);
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
			if(caliNext<2)
			{
				subState=0;
				ledInterval=500;
			}
			else
			{
				LEDSet(0);
				params.pwm_min=(s32)caliTmp[0];
				params.pwm_max=(s32)caliTmp[1];
				ParamWrite();
				
				state=0;
				ledInterval=1000;
				ledFlash=0;
				printf("Cali complete:\r\n");
				printf("Vol:%f %f\r\n",caliTmp[0],caliTmp[1]);
				printf("min=%d\tmax=%d\r\n",params.pwm_min,params.pwm_max);
			}
		}
		caliTmp[caliNext]+=pwmState[3].value;
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
