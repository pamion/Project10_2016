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
	char ptemp[20];
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
	sprintf(ptemp, "%d", __builtin_popcount(publicConfig.channelsToogleMask));
	usart_write_line(USER_RS232, ptemp);
	usart_write_line(USER_RS232, " channels are ON):\r\n");
	
	aux = publicConfig.channelsToogleMask;
	usart_write_line(USER_RS232, "+------+------+------+------+------+------+------+------+\r\n");
	for (i = 0; i < 8; i++ ) {
		sprintf(ptemp, "| Ch%02d ", i);
		usart_write_line(USER_RS232, ptemp);
	}
	usart_write_line(USER_RS232, "|\r\n");
	
	for (i = 0; i < 8; i++ ) {
		usart_write_line(USER_RS232, "| ");
		DISP_ON_OFF_SPACE( (aux & 0x0001) );
		usart_write_line(USER_RS232, "  ");
		aux = aux >> 1;
	}
	usart_write_line(USER_RS232, "|\r\n");
	usart_write_line(USER_RS232, "+------+------+------+------+------+------+------+------+\r\n");
	
	for (i = 8; i < 16; i++ ) {
		sprintf(ptemp, "| Ch%02d ", i);
		usart_write_line(USER_RS232, ptemp);
	}
	usart_write_line(USER_RS232, "|\r\n");
	
	for (i = 8; i < 16; i++ ) {
		usart_write_line(USER_RS232, "| ");
		DISP_ON_OFF_SPACE( (aux & 0x0001) );
		usart_write_line(USER_RS232, "  ");
		aux = aux >> 1;
	}
	usart_write_line(USER_RS232, "|\r\n");
	usart_write_line(USER_RS232, "+------+------+------+------+------+------+------+------+\r\n");
	
	//TODO Dodìlat Output stings, measurement times...
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
	usart_write_line(USER_RS232, "Do you wish to continue? (Y/N)\r\n\r\n");
}


//art_write_line(USER_RS232, "--------- --------- --------- --------- --------- --------- --------- --------- ");