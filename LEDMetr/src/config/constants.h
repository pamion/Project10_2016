/*
 * \file constants.h
 *
 * \brief Define all constants for the whole projects
 *
 */ 


#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#ifdef DEBUG
	#define CONFIG_PASS			"pass"
#else
	#define CONFIG_PASS			"p9vnqat3"
#endif //DEBUG

#define CMD_LINE_SIGN_NORMAL	"\r\n> "
#define CMD_LINE_SIGN_FACTORY	"\r\n#> "

#define RESET_TIME				3000

#define LED_PIN					AVR32_PIN_PA23
#define RESET_BTN				AVR32_PIN_PA26
#define FPBA					12000000
#define Vlog_CONVERSION			65536
#define BrightnessCorrection 	0.01111111
#define N_VALID_DEC_PLACES		3
#define CHARS_PER_SC_NUMBER		7
#define CHARS_PER_RN_NUMBER		2
#define CHARS_PER_NORM_NUMBER	5

//******* ADRead timing (timer/counter) definitions  *******//
#define ADRead_TC               (&AVR32_TC)
#define ADRead_TC_IRQ_GROUP     AVR32_TC_IRQ_GROUP
#define ADRead_TC_IRQ           AVR32_TC_IRQ0
#define TC_CHANNEL				0

//******* USART DEFINITIONS *******//
#define USER_RS232				(&AVR32_USART0)
#define USER_RS232_RX_PIN       AVR32_USART0_RXD_0_0_PIN
#define USER_RS232_RX_FUNCTION  AVR32_USART0_RXD_0_0_FUNCTION
#define USER_RS232_TX_PIN       AVR32_USART0_TXD_0_0_PIN
#define USER_RS232_TX_FUNCTION  AVR32_USART0_TXD_0_0_FUNCTION
#define SER_RS232_RTS_PIN		AVR32_USART0_RTS_0_0_PIN
#define SER_RS232_RTS_FUNCTION	AVR32_USART0_RTS_0_0_FUNCTION
#define SER_RS232_CTS_PIN		AVR32_USART0_CTS_0_0_PIN
#define SER_RS232_CTS_FUNCTION	AVR32_USART0_CTS_0_0_FUNCTION
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

#define ADC_ERROR_CHANNEL		1
#define ADC_ERROR_PIN			AVR32_ADC_AD_1_PIN
#define ADC_ERROR_FUNCTION		AVR32_ADC_AD_1_FUNCTION
#define ADC_ERR_VOLT_HIGH		0.6
#define ADC_ERR_VOLT_LOW		0.4
#define ADC_TO_VOLT(value)		value * 3.3 / 1024

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
#define RS232_BUFF_IN_SIZE		200
#define SUBBUFF_LEN				10
#define UNKNOWN					-1
#define INVALID					-1

/* Validation statuses */
#define VAL_INTEGER				0
#define VAL_HEX					1
#define VAL_OUTPUT_STR			2
#define VAL_BINARY				3

#define VAL_HEX_LEN_ERR			-1
#define VAL_STR_LEN_ERR			-2
#define VAL_STR_QOTATION_ERR	-3
#define VAL_ODD_CHARS			-4
#define VAL_BAD_HEX				-5
#define VAL_BIN_LEN_ERR			-6


//******* MACROs *******//
#define CHECK_BIT(var,pos)				((var) & (1<<(pos)))
#define CHECK_COMMAND(command, level)	(strcmp(subBuff[level], command) == 0)
#define DISP_ON_OFF(var)				if (var == 0x8000) usart_write_line(USER_RS232, "ON"); else usart_write_line(USER_RS232, "OFF")
#define DISP_ON_OFF_SPACE(var)			if (var == 0x8000) usart_write_line(USER_RS232, "ON "); else usart_write_line(USER_RS232, "OFF")

typedef struct {
	char hwMajor[2];
	char hwMinor[2];
	char swMajor[2];
	char swMinor[2];
	char hwSN[12];
	uint16_t settlingTime;
	uint16_t samplingRate;
	uint8_t adClkPresc;
	char reserve1[3];
	float pdSens;
	uint8_t calibOnOff;
	char reserve2;
	uint16_t calibData[9];
} nvram_data_t1;

typedef struct {
	uint32_t comPortBaudrate;
	uint8_t comPortHandshake;
	char reserve1[3];
	uint8_t measNPLC;
	char reserve2[3];
	uint8_t measPowerLineFreq;
	uint8_t measRounding;
	uint8_t measScientific;
	char reserved3;
	uint16_t channelsToogleMask;
	char reserve4[2];
	char outputPrefix[8];
	char outputSeparator[8];
	char outputSuffix[8];
	char outputLineEnding[8];
	
} nvram_data_t2;

//Default values - hidden memory
#define HW_MAJOR_DEFAULT		' ', '1'
#define HW_MINOR_DEFAULT		'0', '4'
#define SW_MAJOR_DEFAULT		' ', '1'
#define SW_MINOR_DEFAULT		'0', '0'
#define HW_SN_DEFAULT			'L', '0', '0', '1', '4', 'B', 'F', '1', '0', '0', '0', '0'
#define SETTLING_TIME_DEFAULT	10
#define SAMPLING_RATE_DEFAULT	1000
#define AD_CLK_PRESC_DEFAULT	2
#define PD_SENS_DEFAULT			9.0
#define CALIB_ON_OFF_DEFAULT	0
#define CALIB_DATA_DEFAULT		0, 0, 0, 0, 0, 0, 0, 0, 0

//Default values - hidden memory
#define COM_PORT_BAUD_DEFAULT	19200
#define COM_PORT_HAND_DEFAULT	0
#define MEAS_NPLC_DEFAULT		3
#define MEAS_PL_FREQ_DEFAULT	50
#define MEAS_ROUNDING_DEFAULT	1
#define MEAS_SCIENTIFIC_DEFAULT 0
#define CHANNELS_MASK_DEFAULT	0xFFFF
#define OUTPUT_PREFIX_DEFAULT	0x02, '\0', 0, 0, 0, 0, 0, 0
#define OUTPUT_SEP_DEFAULT		';', '\0', 0, 0, 0, 0, 0, 0
#define OUTPUT_SUF_DEFAULT		'\0', 0, 0, 0, 0, 0, 0, 0

#ifdef DEBUG
	#define OUTPUT_ENDING_DEFAULT	'\r', '\n', '\0', 0, 0, 0, 0, 0
#else
	#define OUTPUT_ENDING_DEFAULT	0x03, '\0', 0, 0, 0, 0, 0, 0
#endif //DEBUG


#define RESERVE_1B				0
#define RESERVE_2B				0, 0
#define RESERVE_3B				0, 0, 0

#endif /* CONSTANTS_H_ */