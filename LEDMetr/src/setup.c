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