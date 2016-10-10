/*
 * constants.h
 *
 * Created: 09.10.2016 22:01:11
 *  Author: bosti
 */ 


#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define LINE_START				'D'
#define CONFIG_PASS				"p9vnqat3"

#define TEST_LED				AVR32_PIN_PA23
#define FPBA					12000000
#define Vlog_CONVERSION			65536
#define BrightnessCorrection 	0.01111111

//******* ADRead timing (timer/counter) definitions  *******//
#define ADRead_TC               (&AVR32_TC)
#define ADRead_TC_IRQ_GROUP     AVR32_TC_IRQ_GROUP
#define ADRead_TC_IRQ           AVR32_TC_IRQ0
#define TC_CHANNEL				0


//20140602 New - Request from Kozin to speed up writing to RS
#define RS232WritePeriod		1000	// tato hodnota by mela byt minimalne o 1 vetsi jak 16*(NumberOfAveragedValues+MultiplexerSwitchTime)+1 tj. 16*(20+20)+1=641
#define NumberOfAveragedValues	4		// Pocet opakovanych cteni jednoho cidla. Perioda ceteni je fixni = 1ms.
#define MultiplexerSwitchTime	10		// Time reserved for multiplexer channels switching. During this time AD is not read out.
//20140602 End

//20140602 Original
//#define RS232WritePeriod			641	// tato hodnota by mela byt minimalne o 1 vetsi jak 16*(NumberOfAveragedValues+MultiplexerSwitchTime)+1 tj. 16*(20+20)+1=641
//#define NumberOfAveragedValues	20	// Pocet opakovanych cteni jednoho cidla. Perioda cteni je fixni = 1ms.
//#define MultiplexerSwitchTime		20	// Time reserved for multiplexer channels switching. During this time AD is not read out.
//20140602 Original End

//******* USART DEFINITIONS *******//
#define USER_RS232				(&AVR32_USART0)
#define USER_RS232_RX_PIN       AVR32_USART0_RXD_0_0_PIN
#define USER_RS232_RX_FUNCTION  AVR32_USART0_RXD_0_0_FUNCTION
#define USER_RS232_TX_PIN       AVR32_USART0_TXD_0_0_PIN
#define USER_RS232_TX_FUNCTION  AVR32_USART0_TXD_0_0_FUNCTION
#define USER_RS232_IRQ          AVR32_USART0_IRQ

//******* AD SPI DEFINITIONS *******//
#define AD_SPI					(&AVR32_SPI0)
#define AD_SPI_NPCS				0
#define AD_SPI_BAUDRATE			FPBA
#define AD_SPI_CLK_PIN			AVR32_SPI0_SCK_0_0_PIN
#define AD_SPI_CLK_FUNCTION		AVR32_SPI0_SCK_0_0_FUNCTION
#define AD_SPI_MISO_PIN			AVR32_SPI0_MISO_0_0_PIN
#define AD_SPI_MISO_FUNCTION	AVR32_SPI0_MISO_0_0_FUNCTION
#define AD_SPI_MOSI_PIN			AVR32_SPI0_MOSI_0_0_PIN
#define AD_SPI_MOSI_FUNCTION	AVR32_SPI0_MOSI_0_0_FUNCTION
//#define AD_SPI_NPCS_PIN		SPARE_SPI_NPCS_PIN				//AVR32_SPI0_NPCS_0_0_PIN			// Chip (slave) select - not used
//#define AD_SPI_NPCS_FUNCTION	SPARE_SPI_NPCS_FUNCTION			//AVR32_SPI0_NPCS_0_0_FUNCTION
#define AD_SPI_CNV				AVR32_PIN_PA05					// Trigger AD conversion signal - transition L->H

//******* MULTIPLEXER DEFINITIONS *******//
#define MULTIPLEXER_A0_PIN		AVR32_PIN_PA14
#define MULTIPLEXER_A1_PIN		AVR32_PIN_PA15
#define MULTIPLEXER_A2_PIN		AVR32_PIN_PA16
#define MULTIPLEXER_A3_PIN		AVR32_PIN_PA17
#define MULTIPLEXER_EN_PIN		AVR32_PIN_PA18

/* Machine status definitions */
#define MACHINE_MEASURE					0
#define MACHINE_USER_CONFIGURATION		1
#define MACHINE_FACTORY_CONFIGURATION	2

/* RS232 status definitions */
#define RS232_INITIAL			0		//Výchozí stav				- èeká se na první znak
#define RS232_RECIEVING			1		//Pøijímání znakù			- èeká se na ukonèovací znak
#define RS232_READY_TO_PROCESS	2		//Byl pøijat poslední znak	- èeká se na zpracování


//******* MACROs *******//
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define CHECK_COMMAND(command, level) (strcmp(sub_str[level], command) == 0)

#endif /* CONSTANTS_H_ */