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
#include "fcs/rs232TextOut.h"
#include "fcs/serialTask.h"

//******* GLOBAL VARIABLES *******//
volatile uint16_t AD_Data				= 0;
volatile uint16_t AD_Data_Values[16];			// promena pro ukladani hodnot vsech sestnacti kanalu
volatile uint16_t AD_Data_Values2Send[16];		// promena pro odeslání hodnot vsech sestnacti kanalu
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
volatile short int afterFirstQuote		= 0;
volatile char bufferRS232[100];

//Status LuxmMetru
volatile short int statusMachine		= MACHINE_MEASURE;

char pref[9];
char sepa[9];
char suff[9];
char lend[9];

__attribute__((section (".userpage"))) nvram_data_t1 hiddenConfig __attribute__ ((aligned (256))) = {
	.hwMajor			= {HW_MAJOR_DEFAULT},
	.hwMinor			= {HW_MINOR_DEFAULT},
	.swMajor			= {SW_MAJOR_DEFAULT},
	.swMinor			= {SW_MINOR_DEFAULT},
	.hwSN				= {HW_SN_DEFAULT},
	.settlingTime		=  SETTLING_TIME_DEFAULT,
	.reserve1			= {RESERVE_2B},
	.samplingRate		=  SAMPLING_RATE_DEFAULT,
	.reserve2			= {RESERVE_2B},
	.adClkPresc			=  AD_CLK_PRESC_DEFAULT,
	.reserve3			= {RESERVE_3B},
	.pdSens				=  PD_SENS_DEFAULT,
	.calibOnOff			=  CALIB_ON_OFF_DEFAULT,
	.reserve4			=  RESERVE_1B,
	.calibData			= {CALIB_DATA_DEFAULT},
};

__attribute__((section (".userpage"))) nvram_data_t2 publicConfig __attribute__ ((aligned (256))) = {
	.comPortBaudrate	=  COM_PORT_BAUD_DEFAULT,
	.comPortHandshake	=  COM_PORT_HAND_DEFAULT,
	.reserve1			=  RESERVE_1B,
	.measNPLC			=  MEAS_NPLC_DEFAULT,
	.reserve2			= {RESERVE_3B},
	.measPowerLineFreq	=  MEAS_PL_FREQ_DEFAULT,
	.measRounding		=  MEAS_ROUNDING_DEFAULT,
	.measScientific		=  MEAS_SCIENTIFIC_DEFAULT,
	.reserved3			=  RESERVE_1B,
	.channelsToogleMask	=  CHANNELS_MASK_DEFAULT,
	.reserve4			= {RESERVE_2B},
	.outputPrefix		= {OUTPUT_PREFIX_DEFAULT},
	.outputSeparator	= {OUTPUT_SEP_DEFAULT},
	.outputSuffix		= {OUTPUT_SUF_DEFAULT},
	.outputLineEnding	= {OUTPUT_ENDING_DEFAULT}
};


int main (void) {
	short int i;
	short int sendData = 0;

	mainInit();
	
	while (1) {
		//Process all data from terminal, if there is any
		serialTask();
		

		if (print_sec) {
			//If 1ms interrupt has been raised
			print_sec = 0;
			
			if ((statusMachine == MACHINE_MEASURE) && ((tc_tick / RS232WritePeriod)) ) {
				//Copy measured data to send buffer
				for (i = 0; i < 16; i++) {
					AD_Data_Values2Send[i] = AD_Data_Values[i];
				}
				sendData = 1;
			}
			
			if ( (tc_tick == 100) || (tc_tick == 101) ) {
				gpio_set_gpio_pin(TEST_LED);
			}
		}
		
		if ( usart_tx_ready(USER_RS232) && (sendData == 1) ) {
			//if serial line is ready and we have data to send
			sendData = measTask();
			if ( sendData == 0 ) {
				gpio_clr_gpio_pin(TEST_LED);
				tc_tick = 0;	
			}
			
		}
		
	}
}
