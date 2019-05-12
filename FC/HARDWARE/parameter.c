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
	params.phi_k=0;
	params.phi_b=0;
	params.phi_p=0;
	params.phi_i=0;
	params.phi_d=0;
	params.yaw_p=0.05;
	params.aile=0.8f;
	params.elev=0.6f;
	params.phi=0.3f;
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
			params.phi_p=paramReadFixed(v,5);
		break;
		case 2:
			params.phi_i=paramReadFixed(v,5);
		break;
		case 3:
			params.phi_d=paramReadFixed(v,5);
		break;
		case 4:
			params.yaw_p=paramReadFixed(v,5);
		break;
		case 5:
			params.aile=paramReadFixed(v,5);
		break;
		case 6:
			params.elev=paramReadFixed(v,5);
		break;
		case 7:
			params.phi=paramReadFixed(v,5);
		break;
	}
	return ParamWrite();
}

void ParamShow(void)
{
	printf("#0:Reverse:%d\r\n",params.Rev);
	printf("#1:phi_p:%f\r\n",params.phi_p);
	printf("#2:phi_i:%f\r\n",params.phi_i);
	printf("#3:phi_d:%f\r\n",params.phi_d);
	printf("#4:yaw_p:%f\r\n",params.yaw_p);
	printf("#5:aile_mix:%f\r\n",params.aile);
	printf("#6:elev_mix:%f\r\n",params.elev);
	printf("#7:phi_mix:%f\r\n",params.phi);
}
