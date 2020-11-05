#include "parameter.h"
#include "stdio.h"

ParameterSet params;

u16 readHalfWord(u32 addr)
{
	return *(__IO uint16_t*)addr;
}


u8 ParamRead(void)
{
	u16 *data=(u16*)(&params);
	u16 i;
	for(i=0;i<sizeof(params)/2;i++)
	{
		data[i]=readHalfWord(FLASH_BASE+TGT_SECTOR_ADDR+i*2);
	}
	return 0;
}


u8 ParamWrite(void)
{
	u16 *data;
	u16 i;
	FLASH_Unlock();
	u32 sectorAddr=FLASH_BASE+TGT_SECTOR_ADDR;
	FLASH_ErasePage(sectorAddr);
	data=(u16*)(&params);
	for(i=0;i<sizeof(params)/2;i++)
	{
		FLASH_ProgramHalfWord(sectorAddr+i*2,data[i]);
	}
	FLASH_Lock();
	return 0;
}

void ParamReset(void)
{	
	params.headFlag=0xCFCF;
	params.pwm_rate=400;
	params.scale_ratio=5;
	params.man_adv=0.35;
	params.freq_min=5;
	params.freq_max=23;
	params.am_max=150;
	params.bias_max=10;
	params.ratio=7;
	params.dead_zone=0.05;
	params.tailFlag=0xFCFC;
}

float paramReadFixed(s32 v,u8 n)
{
	float res=v;
	while(n--)
	{
		res/=10;
	}
	return res;
}

u8 ParamSet(u8 id,s32 v)
{
	switch(id)
	{
		case 0:
			params.pwm_rate=v;
		break;
		case 1:
			params.scale_ratio=paramReadFixed(v,4);
		break;
		case 2:
			params.man_adv=paramReadFixed(v,5);
		break;
		case 3:
			params.freq_min=paramReadFixed(v,4);
		break;
		case 4:
			params.freq_max=paramReadFixed(v,4);
		break;
		case 5:
			params.am_max=paramReadFixed(v,3);
		break;
		case 6:
			params.bias_max=paramReadFixed(v,3);
		break;
		case 7:
			params.ratio=paramReadFixed(v,4);
		break;
		case 8:
			params.dead_zone=paramReadFixed(v,5);
		break;
	}
	return ParamWrite();
}

void ParamShow(void)
{
	printf("#0:pwm_rate:%d\r\n",params.pwm_rate);
	printf("#1:scale_ratio:%f\r\n",params.scale_ratio);
	printf("#2:man_adv:%f\r\n",params.man_adv);
	printf("#3:freq_min:%f\r\n",params.freq_min);
	printf("#4:freq_max:%f\r\n",params.freq_max);
	printf("#5:am_max:%f\r\n",params.am_max);
	printf("#6:bias_max:%f\r\n",params.bias_max);
	printf("#7:ratio:%f\r\n",params.ratio);
	printf("#8:dead_zone:%f\r\n",params.dead_zone);
	printf("Param end.\r\n");
}
