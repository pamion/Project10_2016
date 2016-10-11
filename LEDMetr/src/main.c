/**
 * \file
 *
 * \brief LED Meter
 * Version: 0.11
 * Author: Pamion & Ondøej Boštík
 *
 *
 */

//#define DEBUG
//#define RELEASE

/*
 * Include header files for all drivers that have been imported from
 * AVR Software Framework (ASF).
 */
#include <avr32/io.h>
#include <avr32/uc3a0512.h>
#include <string.h>
#include <stdio.h> 
#include <asf.h>
#include <math.h>
#include "gpio.h"
#include "power_clocks_lib.h"
#include "cycle_counter.h"
#include "usart.h"
#include "compiler.h"
#include "spi.h"
#include "delay.h"
#include "pm.h"
#include "intc.h"
#include "config/constants.h"
#include "fcs/helpers.h"
#include "fcs/setup.h"
#include "fcs/interrupts.h"

//******* GLOBAL VARIABLES *******//
volatile uint16_t AD_Data				= 0;
volatile uint16_t AD_Data_Values[16];			// promena pro ukladani hodnot vsech sestnacti kanalu
volatile uint8_t MultiplexerChannel		= 0;
uint8_t LastStringLenght				= 0;
//int Error;
double Brightness						= 0;
volatile uint32_t ADReadsSummator		= 0;	// Slouzi k akumulaci opakovanych cteni hodnoty kanalu a spolu s NumberOfAveragedValues slozi k vypoctu aritmetickeho prumeru.
volatile int AveragedReadsCounter		= 0;
volatile int ChannelSwitchingCounter	= 0;
volatile int ChannelSwitchedFlag		= 0;	// 0 - Switching in progress
												// 1 - Channel switching finished

// To specify we have to print a new time
volatile int print_sec					= 1;	// used as flag
volatile U32 tc_tick					= 0;
volatile avr32_tc_t *tc					= ADRead_TC;

//Promìnné pro naèítání øetìzce
volatile short int statusRS232			= RS232_INITIAL;
volatile short int pozRS232				= 0;
volatile char bufferRS232[100];

//Status LuxmMetru
volatile short int statusMachine		= MACHINE_MEASURE;

__attribute__((section (".userpage"))) nvram_data_t1 hiddenConfig __attribute__ ((aligned (256)))
= {
	.hwMajor			= {' ', '1'},
	.hwMinor			= {'0', '4'},
	.swMajor			= {' ', '0'},
	.swMinor			= {'1', '1'},
	.hwSN				= {'L', '0', '0', '1', '4', 'B', 'F', '1', '0', '0', '0', '0'},
	.settlingTime		= {10},
	.reserve1			= {0, 0},
	.samplingRate		= {1000},
	.reserve2			= {0, 0},
	.adClkPresc			= {2},
	.reserve3			= {0, 0, 0},
	.pdSens				= {9.0},
	.calibOnOff			= {0},
	.reserve4			= {0},
	.calibData			= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

__attribute__((section (".userpage"))) nvram_data_t2 publicConfig __attribute__ ((aligned (256)))
= {
	.comPortBaudrate	= {19200},
	.comPortHandshake	= {0},
	.reserve1			= {0},
	.measNPLC			= {3},
	.reserve2			= {0, 0, 0},
	.measPowerLineFreq	= {50},
	.measRounding		= {1},
	.measScientific		= {0},
	.reserved3			= {0},
	.channelsToogleMask	= {0xFF, 0xFF},
	.reserve4			= {0, 0},
	.outputPrefix		= {'0x02', 0, 0, 0, 0, 0, 0, 0},
	.outputSeparator	= {':', '\0', 0, 0, 0, 0, 0, 0},
	.outputSuffix		= {'\n', 0, 0, 0, 0, 0, 0, 0},
	.outputLineEnding	= {0x03, '\0', 0, 0, 0, 0, 0, 0}
};


int main (void)
{
	char ptemp[20];
	
	mainInit();
	
/*	flashc_memcpy((void *)US_HW_major,   &"b", sizeof(US_HW_major[0]),   true);*/
	
	while (1) {
		//Process all data from terminal, if there is any
		serialTask();

		//Show measurements
		if(statusMachine == MACHINE_MEASURE) {
			if ((print_sec) && (!(tc_tick%RS232WritePeriod))) {
				int j;
				// usart_putchar(USER_RS232,0x02);					//STX	
				usart_putchar(USER_RS232,LINE_START);
			
				for (j=0; j<16; j++) {
					ADToBrightness(&Brightness, AD_Data_Values[j]);
					sprintf(ptemp, "%.0f", Brightness);				//Original
					//sprintf(ptemp,"%lu",((uint32_t)Brightness));	//Hella 1.
					//sprintf(ptemp,"%u",AD_Data_Values[j]);		//Hella 2.
					usart_write_line(USER_RS232,ptemp);	
					if (j<15) {
						usart_putchar(USER_RS232, 0x2C);				//Separator ','
					}					
				}
				//usart_putchar(USER_RS232,0x03);					//ETX
				usart_putchar(USER_RS232, 0x0D);					//CR
				usart_putchar(USER_RS232, 0x0A);					//LF					

				print_sec = 0;
				gpio_toggle_pin(TEST_LED);
			} 
		} 
	}
}
