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
	params.Rev=0;
	params.pwm_cali_flag=0;
	params.pwm_min=1500;
	params.pwm_max=1500;
	params.curr_off=0;
	params.curr_gain=10;
	params.volt_gain=5.7;
	params.pwr_p=0;
	params.pwr_i=0;
	params.pwr_d=0;
	params.pwr_i_max=1;
	params.curr_max=30;
	params.power_max=200;
	params.freq=0;
	params.strength=0;
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
			if(v&1)
				params.Rev=1;
			else
				params.Rev=0;
			if(v>9)
				params.Rev|=2;			
		break;
		case 1:
			params.curr_off=paramReadFixed(v,5);
		break;
		case 2:
			params.pwr_p=paramReadFixed(v,5);
		break;
		case 3:
			params.pwr_i=paramReadFixed(v,5);
		break;
		case 4:
			params.pwr_d=paramReadFixed(v,5);
		break;
		case 5:
			params.pwr_i_max=paramReadFixed(v,4);
		break;
		case 6:
			params.curr_max=paramReadFixed(v,4);
		break;
		case 7:
			params.power_max=paramReadFixed(v,3);
		break;
		case 8:
			params.freq=paramReadFixed(v,4);
		break;
		case 9:
			params.strength=paramReadFixed(v,4);
		break;
		case 10:
			params.curr_gain=paramReadFixed(v,4);
		break;
		case 11:
			params.volt_gain=paramReadFixed(v,4);
		break;
	}
	return ParamWrite();
}

void ParamShow(void)
{
	printf("#0:Reverse:%d\r\n",params.Rev);
	printf("#1:curr_off:%f\r\n",params.curr_off);
	printf("#2:pwr_p:%f\r\n",params.pwr_p);
	printf("#3:pwr_i:%f\r\n",params.pwr_i);
	printf("#4:pwr_d:%f\r\n",params.pwr_d);
	printf("#5:pwr_i_max:%f\r\n",params.pwr_i_max);
	printf("#6:curr_max:%f\r\n",params.curr_max);
	printf("#7:power_max:%f\r\n",params.power_max);
	printf("#8:freq:%f\r\n",params.freq);
	printf("#9:strength:%f\r\n",params.strength);
	printf("#10:curr_gain:%f\r\n",params.curr_gain);
	printf("#11:volt_gain:%f\r\n",params.volt_gain);
	printf("Param end.\r\n");
}
