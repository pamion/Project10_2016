/*
 * \file rs232TextOut.c
 * \brief Všechny funkce pro vypsání textù v konfiguraèním módu
 *
 * \author bostik
 *
 */
#include <asf.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "constants.h"
#include "helpers.h"
#include "rs232TextOut.h"

void showInfoText(void) {
	char ptemp[50];
	uint8_t i;
	uint16_t aux;
	
	usart_write_line(USER_RS232, "\r\n \r\nLuxmeter LMX-16 configuration and hardware status:\r\n");
	
	usart_write_line(USER_RS232, "Hardware version: ");
	sprintf(ptemp, "%c%c.%c%c\r\n", hiddenConfig.hwMajor[0], hiddenConfig.hwMajor[1], hiddenConfig.hwMinor[0], hiddenConfig.hwMinor[1]);
	usart_write_line(USER_RS232, ptemp);
	
	usart_write_line(USER_RS232, "Firmware version: ");
	sprintf(ptemp, "%c%c.%c%c\r\n", hiddenConfig.swMajor[0], hiddenConfig.swMajor[1], hiddenConfig.swMinor[0], hiddenConfig.swMinor[1]);
	usart_write_line(USER_RS232, ptemp);
	
	usart_write_line(USER_RS232, "\r\nRS-232 port settings: ");
	sprintf(ptemp, "%d", publicConfig.comPortBaudrate);
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, " baud, RTS/CTS handshaking ");
	DISP_ON_OFF(publicConfig.comPortHandshake);
	usart_write_line(USER_RS232, ".\r\n");
	
	usart_write_line(USER_RS232, "\r\nMeasurement settings: speed ");
	sprintf(ptemp, "%d", publicConfig.measNPLC);
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, " NPLC, line frequency ");
	sprintf(ptemp, "%d", publicConfig.measPowerLineFreq);
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, " Hz, rounding ");
	DISP_ON_OFF(publicConfig.measRounding);
	usart_write_line(USER_RS232, ",\r\nscientific notation ");
	DISP_ON_OFF(publicConfig.measScientific);
	usart_write_line(USER_RS232, ".\r\n");
	
	usart_write_line(USER_RS232, "\r\nChannel status (");
	sprintf(ptemp, "%d", channelCount(publicConfig.channelsToogleMask));
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, " channels are ON):\r\n");
	
	aux = publicConfig.channelsToogleMask;
	usart_write_line(USER_RS232, "+------+------+------+------+------+------+------+------+\r\n");
	for (i = 0; i < 8; i++ ) {
		sprintf(ptemp, "| Ch%02d ", i+1);
		usart_write_line(USER_RS232, ptemp);
	}
	usart_write_line(USER_RS232, "|\r\n");
	
	for (i = 0; i < 8; i++ ) {
		usart_write_line(USER_RS232, "| ");
		DISP_ON_OFF_SPACE( (aux & 0x8000) );
		usart_write_line(USER_RS232, "  ");
		aux = aux << 1;
	}
	usart_write_line(USER_RS232, "|\r\n");
	usart_write_line(USER_RS232, "+------+------+------+------+------+------+------+------+\r\n");
	
	for (i = 8; i < 16; i++ ) {
		sprintf(ptemp, "| Ch%02d ", i+1);
		usart_write_line(USER_RS232, ptemp);
	}
	usart_write_line(USER_RS232, "|\r\n");
	
	for (i = 8; i < 16; i++ ) {
		usart_write_line(USER_RS232, "| ");
		DISP_ON_OFF_SPACE( (aux & 0x8000) );
		usart_write_line(USER_RS232, "  ");
		aux = aux << 1;
	}
	usart_write_line(USER_RS232, "|\r\n");
	usart_write_line(USER_RS232, "+------+------+------+------+------+------+------+------+\r\n\r\n");
	
	usart_write_line(USER_RS232, "Output string settings: Prefix=\"");
	hexToStringRepresentation(publicConfig.outputPrefix);
	usart_write_line(USER_RS232, "\", separator is \"");
	hexToStringRepresentation(publicConfig.outputSeparator);
	usart_write_line(USER_RS232, "\",\r\n");
	usart_write_line(USER_RS232, "  suffix is \"");
	hexToStringRepresentation(publicConfig.outputSuffix);
	usart_write_line(USER_RS232, "\" and line ending is \"");
	hexToStringRepresentation(publicConfig.outputLineEnding);
	usart_write_line(USER_RS232, "\".\r\n");
	
	usart_write_line(USER_RS232, "Example:\r\n");
	outputStringExample( publicConfig.outputPrefix, publicConfig.outputSeparator, publicConfig.outputSuffix, publicConfig.outputLineEnding, publicConfig.measRounding, publicConfig.measScientific );
	usart_write_line(USER_RS232, "\r\n\r\n");

	measTimeInfo( publicConfig.measNPLC, publicConfig.measPowerLineFreq, hiddenConfig.settlingTime, publicConfig.channelsToogleMask, publicConfig.comPortBaudrate );
}

void showConfigText(void) {
	usart_write_line(USER_RS232, "\r\n\r\n");
	usart_write_line(USER_RS232, "--------------------------------------------------\r\n");
	usart_write_line(USER_RS232, "LXM-16 Industrial Luxmeter Configuration Interface\r\n");
	usart_write_line(USER_RS232, "--------------------------------------------------\r\n");
	usart_write_line(USER_RS232, "(c) 2011 - 2016, Pamion, s.r.o., Czech Republic.\r\n");
	usart_write_line(USER_RS232, "www.pamion.cz\r\n\r\n");
	
	usart_write_line(USER_RS232, "Enter \"help\" to get list of available commands.\r\n");
	usart_write_line(USER_RS232, "Enter \"help (command)\" to get available attributes of a command.\r\n");
	usart_write_line(USER_RS232, "Use \"exit\" to save configuration, exit from this menu and resume measurement.\r\n\r\n");
	usart_write_line(USER_RS232, "Use \"discart\" to discart all changes, exit from this menu and resume measurement.\r\n\r\n");
	
	usart_write_line(USER_RS232, "For quick configuration, you can use a batch of commands in the format:\r\n\r\n");
	
	usart_write_line(USER_RS232, "config<newline>\r\n");
	usart_write_line(USER_RS232, "command_1 (attributes)<newline>\r\n");
	usart_write_line(USER_RS232, "command_2 (attributes)<newline>\r\n");
	usart_write_line(USER_RS232, "...\r\n");
	usart_write_line(USER_RS232, "command_N (attributes)<newline>\r\n");
	usart_write_line(USER_RS232, "exit<newline>\r\n\r\n");
	
	usart_write_line(USER_RS232, "CR, LF or CRLF characters are accepted as <newline>.\r\n\r\n");
	
	usart_write_line(USER_RS232, "Use \"expconf\" command to export a configuration batch of current\r\n");
	usart_write_line(USER_RS232, "luxmeter settings.\r\n\r\n");
}

void showDefaultsHelp(void) {
	char ptemp[50];
	char outputPrefix[8]		= {OUTPUT_PREFIX_DEFAULT};
	char outputSeparator[8]		= {OUTPUT_SEP_DEFAULT};
	char outputSuffix[8]		= {OUTPUT_SUF_DEFAULT};
	char outputLineEnding[8]	= {OUTPUT_ENDING_DEFAULT};
	
	usart_write_line(USER_RS232, "\r\nInformation:\r\n\r\n");
	
	usart_write_line(USER_RS232, "This command loads factory defaults of all luxmeter parameters. Specifically,\r\n");
	usart_write_line(USER_RS232, "the settings will default to:\r\n\r\n");
	
	usart_write_line(USER_RS232, "RS-232 port: ");
	sprintf(ptemp, "%d", COM_PORT_BAUD_DEFAULT);
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, " baud, RTS/CTS handshaking ");
	DISP_ON_OFF(COM_PORT_HAND_DEFAULT);
	usart_write_line(USER_RS232, ".\r\n\r\n");
	
	usart_write_line(USER_RS232, "Measurement: speed ");
	sprintf(ptemp, "%d", MEAS_NPLC_DEFAULT);
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, " NPLC, line frequency ");
	sprintf(ptemp, "%d Hz, rounding ", MEAS_PL_FREQ_DEFAULT);
	usart_write_line(USER_RS232, ptemp);
	DISP_ON_OFF(MEAS_ROUNDING_DEFAULT);
	usart_write_line(USER_RS232, ",\r\nscientific notation ");
	DISP_ON_OFF(MEAS_SCIENTIFIC_DEFAULT);
	usart_write_line(USER_RS232, ".\r\n\r\n");
	
	usart_write_line(USER_RS232, "Channels: all 16 channels ON.\r\n\r\n");
	
	usart_write_line(USER_RS232, "Output string: Prefix=\"");
	hexToStringRepresentation(outputPrefix);
	usart_write_line(USER_RS232, "\", separator is \"");
	hexToStringRepresentation(outputSeparator);
	usart_write_line(USER_RS232, "\",\r\n");
	usart_write_line(USER_RS232, "  suffix is \"");
	hexToStringRepresentation(outputSuffix);
	usart_write_line(USER_RS232, "\" and line ending is \"");
	hexToStringRepresentation(outputLineEnding);
	usart_write_line(USER_RS232, "\".\r\n\r\n");
	
	usart_write_line(USER_RS232, "Usage:\r\n");
	usart_write_line(USER_RS232, "       defaults <arguments>\r\n\r\n");

	usart_write_line(USER_RS232, "Arguments:\r\n");
	usart_write_line(USER_RS232, "-s          Suppresses user warning when loading factory defaults. Needed\r\n");
	usart_write_line(USER_RS232, "            for automated configuration of the luxmeter via batch of commands.\r\n\r\n");
}

void showDefaultsWarning(void) {
	char ptemp[50];
	
	usart_write_line(USER_RS232, "Warning! The default settings will take effect after you enter \"exit\"\r\n");
	usart_write_line(USER_RS232, "command. In order to communicate with the luxmeter after that, you will need\r\n");
	usart_write_line(USER_RS232, "to change your terminal program settings accordingly (");
	sprintf(ptemp, "%d baud, RTS/CTS\r\n", COM_PORT_BAUD_DEFAULT);
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, "handshaking ");
	DISP_ON_OFF(COM_PORT_HAND_DEFAULT);
	usart_write_line(USER_RS232, "). Do you wish to continue? (Y/N)\r\n");
	//art_write_line(USER_RS232, "--------- --------- --------- --------- --------- --------- --------- --------- ");
}

void showComportHelp(void) {
	usart_write_line(USER_RS232, "\r\nInformation:\r\n\r\n");
	
	usart_write_line(USER_RS232, "This command can be used to configure RS-232 port baud rate and handshaking\r\n");
	usart_write_line(USER_RS232, "type. When RTS/CTS handshaking is enabled, the luxmeter sends the results only\r\n");
	usart_write_line(USER_RS232, "when permitted (acts as a slave device). However, the luxmeter still performs\r\n");
	usart_write_line(USER_RS232, "measurements in the background and will send the most recent results immediately \r\n");
	usart_write_line(USER_RS232, "after being permitted by the master.\r\n\r\n");
	
	usart_write_line(USER_RS232, "Usage:\r\n");
	usart_write_line(USER_RS232, "       comport <arguments>\r\n\r\n");

	usart_write_line(USER_RS232, "Arguments:\r\n");
	usart_write_line(USER_RS232, "-b (value)    Sets port baud rate. Can be set between 1200 and 115200 baud,\r\n");
	usart_write_line(USER_RS232, "              recommended values are 1200, 2400, 4800, 9600, 14400, 19200 (default\r\n");
	usart_write_line(USER_RS232, "              setting), 28800, 38400, 57600 or 115200 baud.\r\n");
	usart_write_line(USER_RS232, "-h (value)    Enables or disables hardware handshaking (RTS/CTS).\r\n");
	usart_write_line(USER_RS232, "              Accepted values are on or off. Default setting is OFF.\r\n");
	usart_write_line(USER_RS232, "-s            Suppresses all warnings when changing RS-232 port settings.\r\n");
	usart_write_line(USER_RS232, "              Needed for automated configuration of the luxmeter\r\n");
	usart_write_line(USER_RS232, "              via command batch.\r\n\r\n");
	
	usart_write_line(USER_RS232, "Examples:\r\n");
	usart_write_line(USER_RS232, "       comport -b 9600\r\n");
	usart_write_line(USER_RS232, "           Sets RS-232 baud rate to 9600 baud.\r\n");
	usart_write_line(USER_RS232, "       comport -b 9600 -h on -s\r\n");
	usart_write_line(USER_RS232, "           Sets RS-232 baud rate to 9600 baud, enables hardware handshaking\r\n");
	usart_write_line(USER_RS232, "           and suppresses all warnings.\r\n\r\n");
}

void showComportWarning(void) {
	usart_write_line(USER_RS232, "Warning! New RS-232 port settings will take effect after you enter \"exit\"\r\n");
	usart_write_line(USER_RS232, "command. In order to communicate with the luxmeter after that, you will need\r\n");
	usart_write_line(USER_RS232, "to change your terminal program settings accordingly.\r\n");
	usart_write_line(USER_RS232, "Do you wish to continue? (Y/N)\r\n");
}

void showMeasHelp(void) {
	usart_write_line(USER_RS232, "Information:\r\n");
	usart_write_line(USER_RS232, "  This command sets measurement time per channel and numeric format of the\r\n");
	usart_write_line(USER_RS232, "  results. Measurement time greatly affects accuracy and stability of the\r\n");
	usart_write_line(USER_RS232, "  results, especially when measuring illumination levels under 100 lx. Such\r\n");
	usart_write_line(USER_RS232, "  measurements are sensitive to noise, especially the power line noise which\r\n");
	usart_write_line(USER_RS232, "  is ubiquitous in industrial environments. To reduce its influence, measurement\r\n");
	usart_write_line(USER_RS232, "  time is defined as Number of Power Line Cycles (=NPLC). Selecting measurement\r\n");
	usart_write_line(USER_RS232, "  time that is an integer of power line period (cycle) greatly reduces noise\r\n");
	usart_write_line(USER_RS232, "  induced from power lines. Note that different countries use different line\r\n");
	usart_write_line(USER_RS232, "  frequencies (50 or 60 Hz, which translates to 20 ms and 16.6 ms period,\r\n");
	usart_write_line(USER_RS232, "  respectively).\r\n\r\n");
	
	usart_write_line(USER_RS232, "Usage:\r\n");
	usart_write_line(USER_RS232, "       meas <arguments>\r\n\r\n");

	usart_write_line(USER_RS232, "Arguments:\r\n");
	usart_write_line(USER_RS232, "-t  (value)   Sets measurement time the luxmeter spends on each active channel,\r\n");
	usart_write_line(USER_RS232, "              in Number of Power Line Cycles (NPLC). Accepted range is from 1\r\n");
	usart_write_line(USER_RS232, "              to 100 NPLC, only integer values are permissible. Default value is\r\n");
	usart_write_line(USER_RS232, "              3 NPLC, which is regarded as a good tradeoff between measurement\r\n");
	usart_write_line(USER_RS232, "              speed and results accuracy. Value of 1 NPLC is recommended only\r\n");
	usart_write_line(USER_RS232, "              for measuring high levels of illumination (3000 lx or more).\r\n");
	usart_write_line(USER_RS232, "              Values above 20 NPLC are rarely needed even in the harshest\r\n");
	usart_write_line(USER_RS232, "              of industrial environments.\r\n");
	usart_write_line(USER_RS232, "-lf (value)   Sets power line frequency (in Hz) in the country in which the\r\n");
	usart_write_line(USER_RS232, "              luxmeter will be operated. Accepted values are 50 or 60.\r\n");
	usart_write_line(USER_RS232, "              Default value is 50.\r\n");
	usart_write_line(USER_RS232, "-rn on/off    Enables or disables rounding of the measured values to the\r\n");
	usart_write_line(USER_RS232, "              nearest integer number. For example, 12.3 lx will be reported\r\n");
	usart_write_line(USER_RS232, "              as 12 lx and 12.6 lx will be reported as 13 lx. Values lover than\r\n");
	usart_write_line(USER_RS232, "              0.499 lx will be reported as 0 lx. Default setting is ON.\r\n");
	usart_write_line(USER_RS232, "-sc on/off    Enables or disables scientific notation (exponential format) of\r\n");
	usart_write_line(USER_RS232, "              result numbers. Default value is off. When enabled, 0.238 lx will\r\n");
	usart_write_line(USER_RS232, "              be reported as 2.38E-1 and 146,000 lx will be reported as 1.46E5.\r\n");
	usart_write_line(USER_RS232, "              Note that the scientific notation ignores the rounding\r\n");
	usart_write_line(USER_RS232, "              option (-rn switch).\r\n\r\n");
	
	usart_write_line(USER_RS232, "Examples:\r\n");
	usart_write_line(USER_RS232, "       meas -t 1\r\n");
	usart_write_line(USER_RS232, "           Sets measurement time per channel to 1 NPLC. With the default 50 Hz\r\n");
	usart_write_line(USER_RS232, "           line voltage, the luxmeter will spend 20 ms on each active channel.\r\n\r");	
	usart_write_line(USER_RS232, "       meas -t 5 -lf 60\r\n");
	usart_write_line(USER_RS232, "           Sets measurement time per channel to 5 NPLC and indicates that the\r\n");
	usart_write_line(USER_RS232, "           luxmeter will be operated in a country that uses 60 Hz line\r\n");
	usart_write_line(USER_RS232, "           frequency. Therefore, the luxmeter will spend 5 x 16.6 = 83.3 ms\r\n");
	usart_write_line(USER_RS232, "           on each active channel.\r\n");	
	usart_write_line(USER_RS232, "       meas -sc on\r\n");
	usart_write_line(USER_RS232, "           Enables the scientific notation of the results.\r\n\r\n");
}

void showOutputHelp(void) {
	usart_write_line(USER_RS232, "Information:\r\n");
	usart_write_line(USER_RS232, "  This command can be used to configure result string that luxmeter sends\r\n");
	usart_write_line(USER_RS232, "  via RS-232 port. The output string has format:\r\n\r\n");
	
	usart_write_line(USER_RS232, "  <prefix>result1<separator>result2...<separator>result16<suffix><line-ending>\r\n\r\n");

	usart_write_line(USER_RS232, "  The string always contains measured results for all 16 luxmeter channels,\r\n");
	usart_write_line(USER_RS232, "  disabled channels read as zero. Use of period (\".\" character, dot) in any\r\n");
	usart_write_line(USER_RS232, "  part of the output string is strongly discouraged, because it serves as \r\n");
	usart_write_line(USER_RS232, "  decimal mark in the measured results. Prefix, separator and suffix are\r\n");
	usart_write_line(USER_RS232, "  limited to 8 characters length.\r\n\r\n");

	usart_write_line(USER_RS232, "Usage:\r\n");
	usart_write_line(USER_RS232, "       output <arguments>\r\n\r\n");

	usart_write_line(USER_RS232, "Arguments:\r\n");
	usart_write_line(USER_RS232, "-p \"string\"        Defines the prefix as a text string. The string has to be\r\n");
	usart_write_line(USER_RS232, "                   surrounded by quotation marks and can be empty. Default\r\n");
	usart_write_line(USER_RS232, "                   setting is STX (Start of Text character, ASCII code 0x02).\r\n");
	usart_write_line(USER_RS232, "                   Use the \"-pa\" argument for definition of such non-printable\r\n");
	usart_write_line(USER_RS232, "                   characters.\r\n");
	usart_write_line(USER_RS232, "-pa (ASCII codes)  Defines the prefix characters as a sequence of ASCII codes\r\n");
	usart_write_line(USER_RS232, "                   in hexadecimal format. Only even number of numerals (0-9) and\r\n");
	usart_write_line(USER_RS232, "                   characters A-F or a-f are accepted.\r\n");
	usart_write_line(USER_RS232, "-s \"string\"        Defines the separator as a text string. The string has to\r\n");
	usart_write_line(USER_RS232, "                   be surrounded by quotation marks and can be empty, though\r\n");
	usart_write_line(USER_RS232, "                   that is not recommended for obvious reasons. Default\r\n");
	usart_write_line(USER_RS232, "                   setting is \";\" (semicolon). For definition of non-printable\r\n");
	usart_write_line(USER_RS232, "                   characters, use the \"-sa\" argument below.\r\n");
	usart_write_line(USER_RS232, "-sa (ASCII codes)  Defines the separator characters as a sequence of ASCII\r\n");
	usart_write_line(USER_RS232, "                   codes in hexadecimal format.\r\n");
	usart_write_line(USER_RS232, "-u \"string\"        Defines the suffix as a text string. The string has to be\r\n");
	usart_write_line(USER_RS232, "                   surrounded by quotation marks and can be empty. Default\r\n");
	usart_write_line(USER_RS232, "                   setting is none (empty string). For definition of\r\n");
	usart_write_line(USER_RS232, "                   non-printable characters, use the \"-ua\" argument below.\r\n");
	usart_write_line(USER_RS232, "-ua (ASCII codes)  Defines the suffix characters as a sequence of ASCII\r\n");
	usart_write_line(USER_RS232, "                   codes in hexadecimal format.\r\n");
	usart_write_line(USER_RS232, "-l (value)         Selects from several common line ending types. Accepted\r\n");
	usart_write_line(USER_RS232, "                   values are None, ETX (End of Text), EOT (End of\r\n");
	usart_write_line(USER_RS232, "                   Transmission), CR (Carriage Return), LF (Line Feed), CRLF and\r\n");
	usart_write_line(USER_RS232, "                   LFCR. Default value is ETX (End of Text, ASCII code 0x03).\r\n");
	usart_write_line(USER_RS232, "-la (ASCII codes)  Allows definition of non-standard line endings (e.g. an\r\n");
	usart_write_line(USER_RS232, "                   Escape Sequence) via ASCII codes in hexadecimal format.\r\n");
	usart_write_line(USER_RS232, "                   Only even number of numerals (0-9) and characters A-F or a-f\r\n");
	usart_write_line(USER_RS232, "                   are accepted.\r\n\r\n");
	
	usart_write_line(USER_RS232, "Examples:\r\n");
	usart_write_line(USER_RS232, "       output -p \"\"\r\n");
	usart_write_line(USER_RS232, "           Changes the prefix to None (empty string).\r\n");
	usart_write_line(USER_RS232, "       output -s \" \"\r\n");
	usart_write_line(USER_RS232, "           Changes the separator to space.\r\n");
	usart_write_line(USER_RS232, "       output -sa 202c20\r\n");
	usart_write_line(USER_RS232, "           Changes the separator to space-comma-space.\r\n");
	usart_write_line(USER_RS232, "       output -p \"START\" -s \" \" -u \"END\" -l crlf\r\n");
	usart_write_line(USER_RS232, "           The output string will read:\r\n");
	usart_write_line(USER_RS232, "           START[result_1] [result_2] .... [result_16]END\r\n");
	usart_write_line(USER_RS232, "           The line will end with CRLF (Carriage Return and Line Feed)\r\n");
	usart_write_line(USER_RS232, "           characters (0x0d0a in hexadecimal ASCII).\r\n");	 
}

void showChannelsHelp(void) {
	usart_write_line(USER_RS232, "Information:\r\n");
	usart_write_line(USER_RS232, "  This command can individually or en masse enable and disable luxmeter\r\n");
	usart_write_line(USER_RS232, "  channels. Disabling unused channels can considerably speed up measurement\r\n");
	usart_write_line(USER_RS232, "  cycle. All 16 channels are enabled by default. The luxmeter will always send\r\n");
	usart_write_line(USER_RS232, "  all 16 results, but disabled channels will read as zero. If no channels are\r\n");
	usart_write_line(USER_RS232, "  enabled, the luxmeter will stop sending the result strings altogether. See\r\n");
	usart_write_line(USER_RS232, "  \"output\" command for more information about output string formatting.\r\n\r\n");
	
	usart_write_line(USER_RS232, "Usage:\r\n");
	usart_write_line(USER_RS232, "       channels <arguments>\r\n\r\n");
	
	usart_write_line(USER_RS232, "Arguments:\r\n");
	usart_write_line(USER_RS232, "-ce (channel number)  Enables a channel, channel number range is 1 to 16.\r\n");
	usart_write_line(USER_RS232, "                      Channel number can have leading zero, e.g. 03 instead of\r\n");
	usart_write_line(USER_RS232, "                      just 3. Only one channel number is accepted, but you can\r\n");
	usart_write_line(USER_RS232, "                      use more consecutive -ce (channel number) arguments.\r\n");
	usart_write_line(USER_RS232, "-cd (channel number)  Disables a channel.\r\n");
	usart_write_line(USER_RS232, "-ct (channel number)  Toggles a channel.\r\n");
	usart_write_line(USER_RS232, "-cm (mask)            Enables or disables channels according to a binary\r\n");
	usart_write_line(USER_RS232, "                      mask. The mask length must be 16 characters and must\r\n");
	usart_write_line(USER_RS232, "                      contain numerals 0 and 1 only. The leftmost character\r\n");
	usart_write_line(USER_RS232, "                      of the mask controls channel 1, the rightmost controls\r\n");
	usart_write_line(USER_RS232, "                      channel 16.\r\n");
	usart_write_line(USER_RS232, "-ae                   Enables all 16 channels.\r\n");
	usart_write_line(USER_RS232, "-ad                   Disables all 16 channels.\r\n\r\n");


	usart_write_line(USER_RS232, "Examples:\r\n");
	usart_write_line(USER_RS232, "       channels -ae\r\n");
	usart_write_line(USER_RS232, "           Enables all 16 luxmeter channels.\r\n");
	usart_write_line(USER_RS232, "       channels -ad -ce 2 -ce 5\r\n");
	usart_write_line(USER_RS232, "           Disables all channels and then enables\r\n");
	usart_write_line(USER_RS232, "           channels 2 and 5 of the luxmeter.\r\n");
	usart_write_line(USER_RS232, "       channels -cm 0100100000000000\r\n");
	usart_write_line(USER_RS232, "           Results in the same channel configuration\r\n");
	usart_write_line(USER_RS232, "           as in example above, but using channel mask.\r\n");
}

void hexToStringRepresentation(char *input ) {
	int i = 0;
	
	while ( input[i] != '\0' ) {
		
		switch ( input[i] ) {
			case 0x01 :
				usart_write_line(USER_RS232, "<SOH>");
				break;
			case 0x02 :
				usart_write_line(USER_RS232, "<STX>");
				break;
			case 0x03 :
				usart_write_line(USER_RS232, "<ETX>");
				break;
			case 0x04 :
				usart_write_line(USER_RS232, "<EOT>");
				break;
			case 0x05 :
				usart_write_line(USER_RS232, "<ENQ>");
				break;
			case 0x06 :
				usart_write_line(USER_RS232, "<ACK>");
				break;
			case 0x07 :
				usart_write_line(USER_RS232, "<BEL>");
				break;
			case 0x08 :
				usart_write_line(USER_RS232, "<BS>");
				break;
			case 0x09 :
				usart_write_line(USER_RS232, "<HT>");
				break;
			case 0x0a :
				usart_write_line(USER_RS232, "<LF>");
				break;
			case 0x0b :
				usart_write_line(USER_RS232, "<VT>");
				break;
			case 0x0c :
				usart_write_line(USER_RS232, "<FF>");
				break;
			case 0x0d :
				usart_write_line(USER_RS232, "<CR>");
				break;
			case 0x0e :
				usart_write_line(USER_RS232, "<SO>");
				break;
			case 0x0f :
				usart_write_line(USER_RS232, "<SI>");
				break;
			case 0x10 :
				usart_write_line(USER_RS232, "<DLE>");
				break;
			case 0x11 :
				usart_write_line(USER_RS232, "<DC1>");
				break;
			case 0x12 :
				usart_write_line(USER_RS232, "<DC2>");
				break;
			case 0x13 :
				usart_write_line(USER_RS232, "<DC3>");
				break;
			case 0x14 :
				usart_write_line(USER_RS232, "<DC4>");
				break;
			case 0x15 :
				usart_write_line(USER_RS232, "<NAK>");
				break;
			case 0x16 :
				usart_write_line(USER_RS232, "<SYN>");
				break;
			case 0x17 :
				usart_write_line(USER_RS232, "<ETB>");
				break;
			case 0x18 :
				usart_write_line(USER_RS232, "<CAN>");
				break;
			case 0x19 :
				usart_write_line(USER_RS232, "<EM>");
				break;
			case 0x1a :
				usart_write_line(USER_RS232, "<SUB>");
				break;
			case 0x1b :
				usart_write_line(USER_RS232, "<ESC>");
				break;
			case 0x1c :
				usart_write_line(USER_RS232, "<FS>");
				break;
			case 0x1d :
				usart_write_line(USER_RS232, "<GS>");
				break;
			case 0x1e :
				usart_write_line(USER_RS232, "<RS>");
				break;
			case 0x1f :
				usart_write_line(USER_RS232, "<US>");
				break;
			case 0x7f :
				usart_write_line(USER_RS232, "<DEL>");
				break;
			default :
				usart_putchar(USER_RS232, input[i]);

		}
		i++;
	}
}

void outputStringExample( char *pre, char *se, char *su, char *le, short int rn, short int sc ) {
	char ptemp[20];
	int j;
	hexToStringRepresentation(pre);
	
	for ( j=0; j<16; j++) {

		if ( sc == 1 ) {
			sprintf(ptemp, "%1.2E", 0.0);
		} else if ( rn == 1 ) {
			sprintf(ptemp, "%1.0f", 0.0);
		} else{
			sprintf(ptemp, "%1.2f", 0.0);
		}

		usart_write_line(USER_RS232, ptemp);
		
		if (j<15)
			hexToStringRepresentation(se);
		else
			hexToStringRepresentation(su);
	}
	hexToStringRepresentation(le);
}

void measTimeInfo( short int NPLC, short int PLFreq, uint16_t settlingTime, uint16_t toogleMask, uint16_t baudRate ) {
	char ptemp[60];
	int cycle, allCycle, sendTime, charsPerMsg;
	
	cycle = ( 1000.0 * NPLC / PLFreq + settlingTime );
	allCycle = channelCount(toogleMask) * cycle;
	
	//count msg
	if ( publicConfig.measScientific == 1 ) {
		charsPerMsg = CHARS_PER_SC_NUMBER;
	} else if ( publicConfig.measRounding == 1 ) {
		charsPerMsg = CHARS_PER_RN_NUMBER;
	} else {
		charsPerMsg = CHARS_PER_NORM_NUMBER;
	}
	charsPerMsg = 15*strlen(publicConfig.outputSeparator) + 16*charsPerMsg;
	charsPerMsg += strlen(publicConfig.outputPrefix) + strlen(publicConfig.outputSuffix) + strlen(publicConfig.outputLineEnding);
	
	sendTime = ( 1000.0 * charsPerMsg * 10 / baudRate );

	usart_write_line(USER_RS232, "With these settings, one measurement cycle takes approximately ");
	sprintf(ptemp, "%d ms\r\n  (%d ms per channel).\r\n\r\n", allCycle, cycle);
	usart_write_line(USER_RS232, ptemp);

	sprintf(ptemp, "It takes about %d ms ", sendTime );
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, "to send out the entire result string via RS-232 port.\r\n\r\n");

	if ( allCycle < sendTime ) {
		usart_write_line(USER_RS232, "\r\n\r\nERR: It looks like your data cannot be send in time before new data will arrive.\r\n");
		usart_write_line(USER_RS232, "Please slow down your measurement or use quicker serial line.\r\n\r\n\r\n\r\n");
	}
}


//art_write_line(USER_RS232, "--------- --------- --------- --------- --------- --------- --------- --------- ");