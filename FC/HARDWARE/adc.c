#include "adc.h"

void ADCInit(void)
{
	GPIO_InitTypeDef gi;
	ADC_InitTypeDef ai;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	gi.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_0;
	gi.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(GPIOB,&gi);
	
	ADC_DeInit(ADC1);
	ai.ADC_Mode=ADC_Mode_Independent;
	ai.ADC_ScanConvMode=DISABLE;
	ai.ADC_ContinuousConvMode=DISABLE;
	ai.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ai.ADC_DataAlign=ADC_DataAlign_Right;
	ai.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&ai);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_RegularChannelConfig(ADC1,8,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,9,1,ADC_SampleTime_239Cycles5);
	
}

u16 ADCRead(u8 ch)
{	
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	if(ch==0)
		ADC_RegularChannelConfig(ADC1,8,1,ADC_SampleTime_239Cycles5);
	else
		ADC_RegularChannelConfig(ADC1,9,1,ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}

u16 ADCReadFiltered(u8 ch)
{
	u32 res=0;
	u8 i;
	for(i=0;i<64;i++)
	{
		res+=ADCRead(ch);
	}
	return res>>6;
}

float ADCReadVol(u8 ch)
{
	return ADCReadFiltered(ch)*3.3/4096;
}

float ADCReadWithRef(void)
{
	u16 ref,val;
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_RegularChannelConfig(ADC1,17,1,ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	ref=ADC_GetConversionValue(ADC1);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_RegularChannelConfig(ADC1,9,1,ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	val=ADC_GetConversionValue(ADC1);
	return 1.2*val/ref;
}

float ADCReadFilteredWithRef(u8 n)
{
	float res=0;
	u8 i=n;
	for(i=0;i<n;i++)
	{
		res+=ADCReadWithRef();
	}
	return res/n;
}
