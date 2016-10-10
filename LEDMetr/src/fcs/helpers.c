/*
 * helpers.c
 *
 * Created: 09.10.2016 21:34:43
 *  Author: bostik
 */ 

#include <asf.h>
#include <string.h>
#include <string.h>
#include <math.h>
#include "constants.h"
#include "helpers.h"

static char *sub_str[10];				//pole ukazatel� rozd�len�ch podle mezer
static const char s[2] = " ";			//pomocn� prom�nn� pro rozsek�n�
static short int i = 0;					//pomocn� prom�nn� pro indexaci
static short int recognized = TRUE;		//pomocn� prom�nn� pro vyps�n� n�pov�dy o nezn�m�m p��kazu

void setChannelUp (uint8_t *CurrentChannel)
{
	(*CurrentChannel)++;
	*CurrentChannel=(*CurrentChannel)%16;
}

void setChannelDown (uint8_t *CurrentChannel)
{
	if (*CurrentChannel==0)
	{
		*CurrentChannel=15;
	}
	else *CurrentChannel=(*CurrentChannel)--;
}

void ADToBrightness(double *pBrightness, int AD_Data)
{
	double static Vlog=0;					//Logarithmic Voltage at the AD input.
	Vlog=2*((double)AD_Data/((double)Vlog_CONVERSION));
	*pBrightness=BrightnessCorrection*pow(10,((double)5*Vlog));
}

void serialTask(void) {
	if (statusRS232 == RS232_READY_TO_PROCESS) {
		/*usart_write_line(USER_RS232, &bufferRS232);
		usart_putchar(USER_RS232, 0x0D);					//CR
		usart_putchar(USER_RS232, 0x0A);					//LF
		*/
		
		//roz�ez�n� na pod�et�zce podle mezery
		sub_str[i] = strtok(bufferRS232, s);
		while( sub_str[i] != NULL )
		{
			sub_str[++i] = strtok(NULL, s);
		}
		
		//Odstran�n� ��d�c�ch znak�, kdy� se objev� (pro jistotu)
		i = -1;
		while( sub_str[i++] != NULL )
		{
			//Odstran�me znak nov�ho ��dku, pokud by se n�m dostal na konec p��kazu
			sub_str[i][strcspn ( sub_str[i], "\n" )] = '\0';
			//Odstran�me znak "carriage return", pokud by se n�m dostal na konec p��kazu
			sub_str[i][strcspn ( sub_str[i], "\r" )] = '\0';
		}
		
		recognized = FALSE;
		
		//Vykon�n� z�kladn�ch p��kaz�
		if CHECK_COMMAND("help", 0) {
			usart_write_line(USER_RS232, "Showing help\r\n");
			recognized = TRUE;
			
		} else if CHECK_COMMAND("info", 0) {
			usart_write_line(USER_RS232, "Showing info\r\n");
			recognized = TRUE;
			
		} else if CHECK_COMMAND("config", 0) {
			usart_write_line(USER_RS232, "Config mode, not measuring...\r\n");
			statusMachine = MACHINE_USER_CONFIGURATION;
			recognized = TRUE;
		}
		
		// Standartn� konfigura�n� p��kazy
		if (statusMachine >= MACHINE_USER_CONFIGURATION) {
			if CHECK_COMMAND("exit", 0) {
				usart_write_line(USER_RS232, "Closing configuration mode, measuring again...\r\n");
				statusMachine = MACHINE_MEASURE;
				recognized = TRUE;
			}
		}
		
		// Skryt� tov�rn� nastaven�
		if (statusMachine >= MACHINE_FACTORY_CONFIGURATION) {
			
		}
		
		// P��kaz nerozpozn�n
		if (recognized == FALSE) {
			usart_write_line(USER_RS232, "Command not recognized\r\n");
			usart_write_line(USER_RS232, "Write 'help' to see the list of all commands\r\n");
		}
		
		//Uvoln�n� bufferu pro dal�� znak
		statusRS232 = RS232_INITIAL;
	}
}