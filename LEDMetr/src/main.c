/**
 * \file
 *
 * \brief LED Meter
 * Version: 0.11
 * Author: Pamion & bostik
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
#include "fcs/buffer.h"

//******* GLOBAL VARIABLES *******//
volatile uint16_t AD_Data				= 0;
volatile uint16_t AD_Data_Values[16];			// promena pro ukladani hodnot vsech sestnacti kanalu
volatile uint16_t AD_Data_Values2Send[16];		// promena pro odesl�n� hodnot vsech sestnacti kanalu
volatile uint8_t MultiplexerChannel		= 0;
uint8_t LastStringLenght				= 0;
double Brightness						= 0;
volatile uint32_t ADReadsSummator		= 0;	// Slouzi k akumulaci opakovanych cteni hodnoty kanalu a spolu s NumberOfAveragedValues slozi k vypoctu aritmetickeho prumeru.
volatile uint16_t NumberOfAveragedValues= 0;
volatile int AveragedReadsCounter		= 0;
volatile int ChannelSwitchingCounter	= 0;
short int enabledChannels;
volatile int dataReady2send				= FALSE;
volatile int ChannelSwitchedFlag		= FALSE;	// FALSE - Switching in progress
													// TRUE  - Channel switching finished

// To specify we have to print a new time
volatile int print_sec					= 1;	// used as flag
volatile U32 tc_tick					= 0;
volatile avr32_tc_t *tc					= ADRead_TC;

//Prom�nn� pro na��t�n� �et�zce
volatile struct T_buffer buffIn;
volatile char buffInChar[RS232_BUFF_IN_SIZE];

//Status LuxMetru
volatile short int statusMachine		= MACHINE_MEASURE;

__attribute__((section (".userpage"))) nvram_data_t1 hiddenConfig __attribute__ ((aligned (256))) = {
	.hwMajor			= {HW_MAJOR_DEFAULT},
	.hwMinor			= {HW_MINOR_DEFAULT},
	.swMajor			= {SW_MAJOR_DEFAULT},
	.swMinor			= {SW_MINOR_DEFAULT},
	.hwSN				= {HW_SN_DEFAULT},
	.settlingTime		=  SETTLING_TIME_DEFAULT,
	.samplingRate		=  SAMPLING_RATE_DEFAULT,
	.adClkPresc			=  AD_CLK_PRESC_DEFAULT,
	.reserve1			= {RESERVE_3B},
	.pdSens				=  PD_SENS_DEFAULT,
	.calibOnOff			=  CALIB_ON_OFF_DEFAULT,
	.reserve2			=  RESERVE_1B,
	.calibData			= {CALIB_DATA_DEFAULT},
};

__attribute__((section (".userpage"))) nvram_data_t2 publicConfig __attribute__ ((aligned (256))) = {
	.comPortBaudrate	=  COM_PORT_BAUD_DEFAULT,
	.comPortHandshake	=  COM_PORT_HAND_DEFAULT,
	.reserve1			=  {RESERVE_3B},
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
	float ErrVoltage  = -1;
	char ptemp[20];
	
	bufferInit(&buffIn, &buffInChar, RS232_BUFF_IN_SIZE);

	mainInit();
	
	while (1) {
		//Process all data from terminal, if there is any
		serialTask();				

		if (print_sec) {
			//If 1ms interrupt has been raised
			print_sec = 0;
			
			if ( ( statusMachine == MACHINE_MEASURE ) && ( dataReady2send ) ) {
				//Copy measured data to send buffer
				for (i = 0; i < 16; i++) {
					AD_Data_Values2Send[i] = AD_Data_Values[i];
				}
				sendData = 1;
				dataReady2send = FALSE;
			}
			
			if ( (tc_tick == 100) || (tc_tick == 101) ) {
				gpio_set_gpio_pin(TEST_LED);
			}
			
			// Check Analog voltage on control pin for errors during channel switching time
			if ( ChannelSwitchedFlag == FALSE ) {
				adc_start(&AVR32_ADC);
				ErrVoltage = ADC_TO_VOLT( adc_get_value( &AVR32_ADC, ADC_ERROR_CHANNEL ) );
			}
			
		}
		
		if ( usart_tx_ready(USER_RS232) && (sendData == 1 ) ) {
			//if serial line is ready and we have data to send
			
			if ( ( ErrVoltage > ADC_ERR_VOLT_LOW ) && ( ErrVoltage < ADC_ERR_VOLT_HIGH ) ) {
				sendData = measTask();
			} else {
				usart_write_line(USER_RS232, publicConfig.outputPrefix);					//Start of line
				usart_write_line(USER_RS232, "Err01 - check photodiode connections!");
				usart_write_line(USER_RS232, publicConfig.outputSuffix);					//Print suffix
				usart_write_line(USER_RS232, publicConfig.outputLineEnding);					//Print End of line
				sendData = 0;
			}
			
			if ( sendData == 0 ) {
				gpio_clr_gpio_pin(TEST_LED);
				tc_tick = 0;	
			}
		}
		
	}
}
