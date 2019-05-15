#include "rtc.h"
#include "delay.h"
#include "stdio.h"
DateTime time;

u8 RTCInit()
{
	u8 t;	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	
	RCC_LSEConfig(RCC_LSE_ON);
	t=0;
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET&&t<250)
	{
		t++;
		delay_ms(10);
	}
	if(t>=250)
	{
		printf("rtc error\r\n");
		return 1;
	}
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForLastTask();
	RTC_WaitForSynchro();
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();
	RTC_EnterConfigMode();
	RTC_SetPrescaler(32767);
	RTC_WaitForLastTask();
	
	RTC_ExitConfigMode();
	RTC_SetCounter(0);
	//RTCSetTime(12,0,0);
	//RTCSetDate(19,5,1);
	
	return 0;
}

void RTCReadTime(void)
{
	u32 timecnt=0;
	u32 ms=((32767-RTC_GetDivider())*1000)>>15;
	timecnt=RTC->CNTH;
	timecnt<<=16;
	timecnt|=RTC->CNTL;
	time.sec=timecnt%60;
	timecnt/=60;
	time.min=timecnt%60;
	timecnt/=60;
	time.hour=timecnt%24;
	timecnt/=24;
	time.day=timecnt;
	time.year=0;
	time.month=0;	
	time.ms=ms;
}
 
