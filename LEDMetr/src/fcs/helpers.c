/*
 * helpers.c
 *
 * Created: 09.10.2016 21:34:43
 *  Author: bostik
 */ 

#include <asf.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "constants.h"
#include "helpers.h"



void setChannelUp (uint8_t *CurrentChannel)
{
	(*CurrentChannel)++;
	*CurrentChannel=(*CurrentChannel)%16;
}

void setChannelDown (uint8_t *CurrentChannel)
{
	if (*CurrentChannel==0)
	{
		*CurrentChannel=15;
	}
	else *CurrentChannel=(*CurrentChannel)--;
}

void ADToBrightness(double *pBrightness, int AD_Data)
{
	double static Vlog=0;					//Logarithmic Voltage at the AD input.
	Vlog=2*((double)AD_Data/((double)Vlog_CONVERSION));
	*pBrightness=BrightnessCorrection*pow(10,((double)5*Vlog));
}

void measTask(void){
	int j;
	char ptemp[20];

	usart_write_line(USER_RS232, pref);					//Start of line
				
	for (j=0; j<16; j++) {
		ADToBrightness(&Brightness, AD_Data_Values[j]);
		sprintf(ptemp, "%.0f", Brightness);				//Print one value
		usart_write_line(USER_RS232, ptemp);
		if (j<15)
			usart_write_line(USER_RS232, sepa);			//Print separator
		else
			usart_write_line(USER_RS232, suff);			//Print suffix
	}
	usart_write_line(USER_RS232, lend);					//Print End of line
				
	print_sec = 0;
	gpio_toggle_pin(TEST_LED);
}