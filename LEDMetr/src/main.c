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
#include "constants.h"
#include "helpers.h"
#include "setup.h"

//******* GLOBAL VARIABLES *******//
volatile uint16_t AD_Data				= 0;
volatile uint16_t AD_Data_Values[16];			// promena pro ukladani hodnot vsech sestnacti kanalu
volatile uint8_t MultiplexerChannel		= 0;
uint8_t LastStringLenght				= 0;
int Error;
double Brightness						= 0;
volatile uint32_t ADReadsSummator		= 0;	// Slouzi k akumulaci opakovanych cteni hodnoty kanalu a spolu s NumberOfAveragedValues slozi k vypoctu aritmetickeho prumeru.
volatile int AveragedReadsCounter		= 0;
volatile int ChannelSwitchingCounter	= 0;
volatile int ChannelSwitchedFlag		= 0;	// 0 - Switching in progress
												// 1 - Channel switching finished

// To specify we have to print a new time
volatile static int print_sec			= 1;	// used as flag
volatile U32 tc_tick					= 0;
volatile avr32_tc_t *tc					= ADRead_TC;

/*
 * \brief Pøerušení pro zpracování A/D pøevodu
 *
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = ADRead_TC_IRQ_GROUP, 1
__interrupt
#endif
static void ADRead_irq(void)
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

/*
 * \brief Pøerušení pøi pøíchodu zprávy po RS-232
 *
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
static void usart_int_handler(void)
{
	int c;

	usart_read_char(USER_RS232, &c);

	if(c == 0x0A) {
		usart_write_line(USER_RS232,"NEW LINE\r\n");	
	}
	// Print the received character to USART. This is a simple echo.
	usart_write_char(USER_RS232, c);
}

/*
 * \brief Pøipraví celý MCU pro bìh
 *
 */
static void mainInit(void) {
	board_init();
	sysclk_init();
	delay_init(FOSC0);

	// Disable all interrupts.
	Disable_global_interrupt();

	// init the interrupts
	INTC_init_interrupts();

	// Options for waveform generation.
	static const tc_waveform_opt_t WAVEFORM_OPT = {
		.channel  = TC_CHANNEL,                        // Channel selection.

		.bswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOB.
		.beevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOB.
		.bcpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOB.
		.bcpb     = TC_EVT_EFFECT_NOOP,                // RB compare effect on TIOB.

		.aswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOA.
		.aeevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOA.
		.acpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOA: toggle.
		.acpa     = TC_EVT_EFFECT_NOOP,                // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

		.wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
		.enetrg   = FALSE,                             // External event trigger enable.
		.eevt     = 0,                                 // External event selection.
		.eevtedg  = TC_SEL_NO_EDGE,                    // External event edge selection.
		.cpcdis   = FALSE,                             // Counter disable when RC compare.
		.cpcstop  = FALSE,                             // Counter clock stopped with RC compare.

		.burst    = FALSE,                             // Burst signal selection.
		.clki     = FALSE,                             // Clock inversion.
		.tcclks   = TC_CLOCK_SOURCE_TC3                // Internal source clock 3, connected to fPBA / 8.
	};

	static const tc_interrupt_t TC_INTERRUPT = {
		.etrgs = 0,
		.ldrbs = 0,
		.ldras = 0,
		.cpcs  = 1,
		.cpbs  = 0,
		.cpas  = 0,
		.lovrs = 0,
		.covfs = 0
	};

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	INTC_register_interrupt(&ADRead_irq, ADRead_TC_IRQ, AVR32_INTC_INT0);

	// Initialize the timer/counter.
	tc_init_waveform(tc, &WAVEFORM_OPT);         // Initialize the timer/counter waveform.

	// Set the compare triggers.
	// Remember TC counter is 16-bits, so counting second is not possible with fPBA = 12 MHz.
	// We configure it to count ms.
	// We want: (1/(fPBA/8)) * RC = 0.001 s, hence RC = (fPBA/8) / 1000 = 1500 to get an interrupt every 1 ms.
	tc_write_rc(tc, TC_CHANNEL, (FPBA / 8) / 1000); // Set RC value.

	tc_configure_interrupts(tc, TC_CHANNEL, &TC_INTERRUPT);

	//******* USER_LED GPIO SETTINGS *******//
	gpio_configure_pin(TEST_LED,GPIO_DIR_OUTPUT);

	static const gpio_map_t RS232_GPIO_MAP =
	{
		{USER_RS232_RX_PIN, USER_RS232_RX_FUNCTION},
		{USER_RS232_TX_PIN, USER_RS232_TX_FUNCTION}
	};

	//******* RS232 Setting *******//
	static const usart_options_t RS232_OPTIONS =
	{
		.baudrate     = 19200, //9600,//19200,//57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	gpio_enable_module(RS232_GPIO_MAP,
	sizeof(RS232_GPIO_MAP)/sizeof(RS232_GPIO_MAP[0]) );

	usart_init_rs232(USER_RS232, &RS232_OPTIONS, FPBA);

	INTC_register_interrupt(&usart_int_handler, USER_RS232_IRQ,
	AVR32_INTC_INT0);

	// Enable USART Rx interrupt.
	USER_RS232->ier = AVR32_USART_IER_RXRDY_MASK;

	// Enable all interrupts.
	Enable_global_interrupt();

	gpio_set_gpio_pin(TEST_LED);

	enable_Multiplexer();
	enable_AD_spi();

	gpio_clr_gpio_pin(MULTIPLEXER_A0_PIN);
	gpio_clr_gpio_pin(MULTIPLEXER_A1_PIN);
	gpio_clr_gpio_pin(MULTIPLEXER_A2_PIN);
	gpio_clr_gpio_pin(MULTIPLEXER_A3_PIN);
	gpio_set_gpio_pin(MULTIPLEXER_EN_PIN);

	gpio_clr_gpio_pin(AD_SPI_CNV);

	SwitchMultiplexerToChannel(&MultiplexerChannel);

	// Start the timer/counter.
	tc_start(tc, TC_CHANNEL);                    // Start 1ms timer/counter.
}


int main (void)
{
	char ptemp[20];
	
	mainInit();
	
	while (1)
	{

//******* Update the display on USART every second. *******//
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
				if (j<15)
				{
				usart_putchar(USER_RS232, 0x2C);		//Separator ','
				}					
			}
			//usart_putchar(USER_RS232,0x03);			//ETX
			usart_putchar(USER_RS232, 0x0D);			//CR
			usart_putchar(USER_RS232, 0x0A);			//LF					

			print_sec = 0;
			gpio_toggle_pin(TEST_LED);
		}
	}
}
