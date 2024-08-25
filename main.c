// thu vien
#include<sn8f5762.h>
#include<intrins.h>

#define ADCInRefVDD (1 << 2) // xac dinh tham chieu nguon
#define ADCClkFosc (0 << 7) 
#define ADCSpeedDiv1 (0 << 0) 	//ADC clock = fosc/1 
#define ADCSpeedDiv2 (1 << 0) 	//ADC clock = fosc/2 
#define ADCSpeedDiv4 (2 << 0) 	//ADC clock = fosc/4 
#define ADCSpeedDiv8 (3 << 0) 	//ADC clock = fosc/8 
#define ADCSpeedDiv16 (4 << 0) 	//ADC clock = fosc/16 
#define ADCSpeedDiv32 (5 << 0) 	//ADC clock = fosc/32 
#define ADCSpeedDiv64 (6 << 0) 	//ADC clock = fosc/64 
#define ADCSpeedDiv128 (7 << 0) //ADC clock = fosc/128 
#define ADCChannelEn (1 << 6) // bat nguon ADC
#define SelAIN4 (4 << 0)
#define SelAIN5 (5 << 0)
#define ADCStart (1 << 6)
#define ADCEn (1 << 7)
#define ClearEOC 0xDF

#define CLR_ADCCH           (ADM & 0xF0)        // clear CHS[3:0]
#define _EOC                (ADM & 0x20) 

#define Hot_upper_value 350 		// Nhiet do 90 do
#define Hot_lower_value 401 		// Nhiet do 85 do
#define Cold_upper_value 2738 	// Nhiet do 10 do
#define Cold_lower_value 2956 	// Nhiet do 5 do

void ADCinit(void)
{
	P0M &= 0xFC; // Thiet lap chan P01M, P02M input
	P0CON |= 0x11; // Thiet lap 2 chan chi doc tin hieu analog thuan tuy
	P0UR &=0xFC;
	
	ADR = ADCClkFosc | ADCChannelEn; // bat nguon ADC
	ADCAL = ADCSpeedDiv128;  				 // chon toc do xu ly
	// Select Reference Voltage
	VREFH = ADCInRefVDD;
	
	// enable ADC.
	ADM |= ADCEn;
}

void ADCStartCovert(void)
{
	ADM |= ADCStart;
}

void ADC_Select(unsigned int channel)
{
	switch (channel)
	{
		case 0: 
		{
			ADM = CLR_ADCCH | SelAIN4;
			break;
		}
		case 1: 
		{
			ADM = CLR_ADCCH | SelAIN5;
			break;
		}
		default: break;
	}
}

void delay(unsigned int n)
{
	int i=0;
	int j=0;
	for (i=0; i<=n; i++)
		for (j=0; j<220; j++);
		{
			_nop_(); _nop_();
			_nop_(); _nop_();
			_nop_(); _nop_();
		}
}

void main(void)
{
	unsigned int adc[2];
	ADCinit();
	P1M = 0xFF;
	while (1)
	{
		ADC_Select(0);
		ADCStartCovert();
		while (_EOC)
		{
			ADM &= ClearEOC;
			adc[0] = (ADB << 4) + (ADM & 00001111);
		}
		ADC_Select(1);
		ADCStartCovert();
		while (_EOC)
		{
			ADM &= ClearEOC;
			adc[1] = (ADB << 4) + (ADM & 00001111);
		}
		if(adc[0] > Hot_upper_value)
				{
					// Tat hot block
					P14 = 0;
				}
				else if (adc[0] < Hot_lower_value)
				{
					// Bat hot block
					P14 = 1;
				}
			if(adc[1] > Cold_upper_value)
				{
					// Bat cold block
					P15 = 1;
				}
				else if(adc[1] < Cold_lower_value)
				{
					// Tat cold block
					P15 = 0;
				}
	}
}	
