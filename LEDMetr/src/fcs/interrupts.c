 /*
 * \file interrupts.c
 * \brief Obsluha pøerušení
 *
 * \author bostik
 *
 */

#include <asf.h>
#include <string.h>
#include <string.h>
#include <math.h>
#include "constants.h"
#include "helpers.h"
#include "setup.h"
#include "buffer.h"
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
	
	if (enabledChannels > 0) {
		if ( ChannelSwitchedFlag ) {
			//******* AD value Read *******//
			gpio_set_gpio_pin(AD_SPI_CNV);
			cpu_delay_cy(2);						// measured by analyzer and this delay is in real appr 4us.
			gpio_clr_gpio_pin(AD_SPI_CNV);
			spi_write(AD_SPI, AD_Data);
			spi_read(AD_SPI, (uint16_t *)&AD_Data);
			ADReadsSummator=ADReadsSummator+AD_Data;
			AveragedReadsCounter++;
		
			//Calculate mean value
			if( AveragedReadsCounter == NumberOfAveragedValues ) {
				AD_Data_Values[MultiplexerChannel]=ADReadsSummator/NumberOfAveragedValues;
			
				if ( setChannelUp((uint8_t *)&MultiplexerChannel) == TRUE ) {
					dataReady2send = TRUE;
				}
				SwitchMultiplexerToChannel((uint8_t *)&MultiplexerChannel);
				ADReadsSummator=0;
				AveragedReadsCounter=0;
				ChannelSwitchedFlag = FALSE;
			}
		
		} else {
			if ( ( ChannelSwitchingCounter < hiddenConfig.settlingTime ) && (enabledChannels > 1) ) { // enabledChannels podmínka -> aby probìhla kontrola chybového napìtí
				ChannelSwitchingCounter++;
			} else {
				ChannelSwitchedFlag = TRUE;	
				ChannelSwitchingCounter = 0;
			}
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
	
	bufferWriteChar ((struct T_buffer*)&buffIn, (char *)c);
}