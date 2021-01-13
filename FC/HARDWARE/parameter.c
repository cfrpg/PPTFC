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
	params.thro_min=1200;
	params.thro_homing=1190;
	params.dead_zone=20;
	params.roll_delay=0.5;
	params.roll_step_val=100;
	params.roll_span=0.5;
	params.ratio=7;
	//params.dead_zone=0.05;
	params.cpg_am=20;
	params.yaw_scale=0.8;
	params.motor_freq_max=50;
	params.ppm_enabled=0;
	params.update_rate=7;
	params.input_rev=0;
	params.flight_mode=1;
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
u32 paramReadBinary(s32 v)
{
	u32 res=0;
	u32 mask=1;
	while(v>0)
	{
		if(v%10==1)
		{
			res|=mask;
		}
		mask<<=1;
		v/=10;
	}
	return res;
}

u8 ParamSet(u8 id,s32 v)
{
	switch(id)
	{
		case 0:
			params.pwm_rate=v;
			if(params.pwm_rate>450)
				params.pwm_rate=450;
			break;
		case 1:
			params.thro_min=v;
			break;
		case 2:
			params.thro_homing=v;
			break;
		case 3:
			params.dead_zone=v;
			break;
		case 4:
			params.roll_delay=paramReadFixed(v,5);
			break;
		case 5:
			params.roll_step_val=v;
			break;
		case 6:
			params.roll_span=paramReadFixed(v,5);
			break;
		case 7:
			params.ratio=paramReadFixed(v,4);
			break;
		case 8:
			params.dead_zone=paramReadFixed(v,5);
			break;
		case 9:
			params.cpg_am=paramReadFixed(v,3);
			break;
		case 10:
			params.yaw_scale=paramReadFixed(v,5);
			if(params.yaw_scale>0.95)
				params.yaw_scale=0.95;
			break;
		case 11:
			params.motor_freq_max=paramReadFixed(v,3);
			break;
		case 12:
			params.ppm_enabled=v;
			break;
		case 13:
			params.update_rate=v;
			break;
		case 14:
			params.input_rev=paramReadBinary(v);
			break;
		case 15:
			params.flight_mode=v;
			break;
	}
	return ParamWrite();
}

void ParamShow(void)
{
	printf("#0:pwm_rate:%d\r\n",params.pwm_rate);
	printf("#1:thro_min:%d\r\n",params.thro_min);
	printf("#2:thro_homing:%d\r\n",params.thro_homing);
	printf("#3:dead_zone:%d\r\n",params.dead_zone);
	printf("#4:roll_delay:%f\r\n",params.roll_delay);
	printf("#5:roll_step_val:%d\r\n",params.roll_step_val);
	printf("#6:roll_span:%f\r\n",params.roll_span);
	printf("#7:ratio:%f\r\n",params.ratio);
	printf("#8:rev:%f\r\n",params.rev);
	printf("#9:cpg_am:%f\r\n",params.cpg_am);
	printf("#10:yaw_scale:%f\r\n",params.yaw_scale);
	printf("#11:motor_freq_max:%f\r\n",params.motor_freq_max);
	printf("#12:ppm_enabled:%d\r\n",params.ppm_enabled);
	printf("#13:update_rate:%d\r\n",params.update_rate);
	printf("#14:input_rev:%d\r\n",params.input_rev);
	printf("#15:flight_mode:%d\r\n",params.flight_mode);
	printf("Param end.\r\n");
}
