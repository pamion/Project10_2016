/*
 * interrupts.c
 *
 * Created: 09.10.2016 22:43:48
 *  Author: bosti
 */ 

#include <asf.h>
#include <string.h>
#include <string.h>
#include <math.h>
#include "constants.h"
#include "helpers.h"
#include "setup.h"
#include "interrupts.h"

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = ADRead_TC_IRQ_GROUP, 1
__interrupt
#endif
void ADRead_irq(void)
{
	// Increment the ms seconds counter
	tc_tick++;

	// specify that an interrupt has been raised
	print_sec = 1;
	
	if (ChannelSwitchedFlag)
	{
		//******* AD value Read *******//
		gpio_set_gpio_pin(AD_SPI_CNV);
		cpu_delay_cy(2);						// measured by analyzer and this delay is in real appr 4us.
		gpio_clr_gpio_pin(AD_SPI_CNV);
		spi_write(AD_SPI, AD_Data);
		spi_read(AD_SPI, &AD_Data);
		ADReadsSummator=ADReadsSummator+AD_Data;
		AveragedReadsCounter++;
		if(AveragedReadsCounter==NumberOfAveragedValues)
		{
			AD_Data_Values[MultiplexerChannel]=ADReadsSummator/NumberOfAveragedValues;
			setChannelUp(&MultiplexerChannel);
			SwitchMultiplexerToChannel(&MultiplexerChannel);
			ADReadsSummator=0;
			AveragedReadsCounter=0;
			ChannelSwitchedFlag=0;
		}
		
	}else
	{
		if(ChannelSwitchingCounter!=MultiplexerSwitchTime)
		{
			ChannelSwitchingCounter++;
		}else
		{
			ChannelSwitchedFlag=1;	
			ChannelSwitchingCounter=0;
		}		
		
	}			
	
	// Clear the interrupt flag. This is a side effect of reading the TC SR.
	tc_read_sr(ADRead_TC, TC_CHANNEL);
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
void usart_int_handler(void)
{
	int c;

	usart_read_char(USER_RS232, &c);

	if(c == 0x0D || c == 0x0A) {					//if CR || LF
		if (statusRS232 == RS232_RECIEVING) {		//pokud nepøišel žádný ukonèovací znak
			statusRS232				= RS232_READY_TO_PROCESS;
			bufferRS232[pozRS232]	= '\0';
			pozRS232				= 0;			
		}
	} else if (c == 0x7F || c == 0x08) {	//if DELETE or BACKSPACE
		pozRS232--;
	} else if (c == 0x20 || c == 0x2D || (c >= 0x30 && c <= 0x39) ||	// if mezerník || pomlèka || èíslo
			  (c >= 0x41 && c <= 0x5A)|| (c >= 0x61 && c <= 0x7A) ) {	// || velké písmeno || malé písmeno
																		//		=> WHITE LIST
		
		if (statusRS232 != RS232_READY_TO_PROCESS) {					// pokud èeká pøíkaz na zpracování, poèkej si
																		// TODO: zkontrolovat dávkové pøíkazy
			bufferRS232[pozRS232++]		= c;
			statusRS232					= RS232_RECIEVING;
		}
	}
}