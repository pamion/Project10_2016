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