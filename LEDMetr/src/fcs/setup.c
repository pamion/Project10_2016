/*
 * setup.c
 *
 * Created: 09.10.2016 21:57:33
 *  Author: bosti
 */ 

#include <asf.h>
#include <string.h>
#include <string.h>
#include <math.h>
#include "setup.h"
#include "constants.h"
#include "helpers.h"
#include "interrupts.h"

void enable_Multiplexer(void)
{
	static const gpio_map_t MULTIPLEXER_GPIO_MAP =
	{
		{MULTIPLEXER_A0_PIN, GPIO_DIR_OUTPUT},
		{MULTIPLEXER_A1_PIN, GPIO_DIR_OUTPUT},
		{MULTIPLEXER_A2_PIN, GPIO_DIR_OUTPUT},
		{MULTIPLEXER_A3_PIN, GPIO_DIR_OUTPUT},
		{MULTIPLEXER_EN_PIN, GPIO_DIR_OUTPUT}	
	};
	
	gpio_enable_module(MULTIPLEXER_GPIO_MAP, sizeof(MULTIPLEXER_GPIO_MAP)/sizeof(MULTIPLEXER_GPIO_MAP[0]));
}

void enable_AD_spi(void)
{
	//******* GPIO PINS MAP *******//
	static const gpio_map_t AD_SPI_GPIO_MAP =
	{
		{AD_SPI_CLK_PIN, AD_SPI_CLK_FUNCTION},
		{AD_SPI_MISO_PIN, AD_SPI_MISO_FUNCTION},
		{AD_SPI_MOSI_PIN, AD_SPI_MOSI_FUNCTION}	
//		{AD_SPI_NPCS_PIN, AD_SPI_NPCS_FUNCTION} //not used
	};
	//******* SPI OPTIONS *******//
	spi_options_t AD_SPI_OPTIONS =
	{
		.reg          = AD_SPI_NPCS,
		.baudrate     = (FPBA/2),
		.bits         = 16,
		.spck_delay   = 0,
		.trans_delay  = 0,
		.stay_act     = 1,
		.spi_mode     = 0,
		.modfdis      = 1
	};
	
	//******* Asign I/O to SPI *******//
	gpio_enable_module(AD_SPI_GPIO_MAP, sizeof(AD_SPI_GPIO_MAP)/sizeof(AD_SPI_GPIO_MAP[0]));
	
	//******* INICIALIZE SPI as Master
	if(spi_initMaster(AD_SPI, &AD_SPI_OPTIONS)!= SPI_OK);// usart_write_line(USER_RS232,"AD_SPI spi_initMaster failure");
	
	//******* Set SPI selection mode: variable_ps, pcs_decode, delay.
	if(spi_selectionMode(AD_SPI, 1, 0, 0)!= SPI_OK);//usart_write_line(USER_RS232,"AD_SPI spi_selectionMode failure");
	
	//******* Enable SPI module. *******//
	spi_enable(AD_SPI);
	
	if(spi_setupChipReg(AD_SPI, &AD_SPI_OPTIONS, FPBA));//usart_write_line(USER_RS232,"AD_SPI spi_ssetupChipReg failure");

}

void SwitchMultiplexerToChannel(uint8_t *Channel)
{
	if(CHECK_BIT(*Channel,0))
	{
		gpio_set_gpio_pin(MULTIPLEXER_A0_PIN);
	}else
	{
		gpio_clr_gpio_pin(MULTIPLEXER_A0_PIN);
	}
	if(CHECK_BIT(*Channel,1))
	{
		gpio_set_gpio_pin(MULTIPLEXER_A1_PIN);
	}else
	{
		gpio_clr_gpio_pin(MULTIPLEXER_A1_PIN);
	}
	if(CHECK_BIT(*Channel,2))
	{
		gpio_set_gpio_pin(MULTIPLEXER_A2_PIN);
	}else
	{
		gpio_clr_gpio_pin(MULTIPLEXER_A2_PIN);
	}
	if(CHECK_BIT(*Channel,3))
	{
		gpio_set_gpio_pin(MULTIPLEXER_A3_PIN);
	}else
	{
		gpio_clr_gpio_pin(MULTIPLEXER_A3_PIN);
	}
		
}

void mainInit(void) {
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
	const usart_options_t RS232_OPTIONS =
	{
		.baudrate     = publicConfig.comPortBaudrate, //9600,//19200,//57600,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	gpio_enable_module(RS232_GPIO_MAP,
	sizeof(RS232_GPIO_MAP)/sizeof(RS232_GPIO_MAP[0]) );

	//start right type on RS232
	usart_init_rs232(USER_RS232, &RS232_OPTIONS, FPBA);
	/*
	if (publicConfig.comPortHandshake == 0) {
		usart_init_rs232(USER_RS232, &RS232_OPTIONS, FPBA);
	} 
	else {
		usart_init_hw_handshaking(USER_RS232, &RS232_OPTIONS, FPBA);
	}*/
	
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
	
	strncpy(pref, publicConfig.outputPrefix,	 8);
	strncpy(sepa, publicConfig.outputSeparator,	 8);
	strncpy(suff, publicConfig.outputSuffix,	 8);
	strncpy(lend, publicConfig.outputLineEnding, 8);
	pref[8]	= '\0';
	sepa[8]	= '\0';
	suff[8]	= '\0';
	lend[8]	= '\0';
}
