/*
 * helpers.c
 *
 * Created: 09.10.2016 21:34:43
 *  Author: bostik
 */ 

#include <asf.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "constants.h"
#include "helpers.h"
#include "rs232TextOut.h"

static char *sub_str[10];				//pole ukazatel� rozd�len�ch podle mezer
static const char s[2] = " ";			//pomocn� prom�nn� pro rozsek�n�
static short int i;						//pomocn� prom�nn� pro indexaci
static short int paramsCount;
static short int recognized = TRUE;		//pomocn� prom�nn� pro vyps�n� n�pov�dy o nezn�m�m p��kazu
static char ptemp[20];					//pomocn� prom�nn� pro v�pis

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
		//roz�ez�n� na pod�et�zce podle mezery
		i = 0;
		sub_str[i] = strtok(bufferRS232, s);
		while( sub_str[i] != NULL ) {
			sub_str[++i] = strtok(NULL, s);
		}
		
		//Odstran�n� ��d�c�ch znak�, kdy� se objev� (pro jistotu)
		i = 0;
		while( sub_str[i++] != NULL ) {
			//Odstran�me znak nov�ho ��dku, pokud by se n�m dostal na konec p��kazu
			sub_str[i][strcspn ( sub_str[i], "\n" )] = '\0';
			//Odstran�me znak "carriage return", pokud by se n�m dostal na konec p��kazu
			sub_str[i][strcspn ( sub_str[i], "\r" )] = '\0';
		}
		
		paramsCount = i - 1; 
		recognized = FALSE;
		
		//Vykon�n� z�kladn�ch p��kaz�
		if CHECK_COMMAND("help", 0) {
			usart_write_line(USER_RS232, "Showing help\r\n");
			recognized = TRUE;
			
		} else if CHECK_COMMAND("info", 0) {
			showInfoText();
			recognized = TRUE;
			
		} else if CHECK_COMMAND("config", 0) {
			if CHECK_COMMAND(CONFIG_PASS, 1) {
				statusMachine = MACHINE_FACTORY_CONFIGURATION;
				usart_write_line(USER_RS232, "\r\n\r\nHIDDEN FACTORY SETTINGS:\r\n");
			} else {
				statusMachine = MACHINE_USER_CONFIGURATION;
				showConfigText();
			}		
			recognized = TRUE;

		}
		
		// Standartn� konfigura�n� p��kazy
		if (statusMachine >= MACHINE_USER_CONFIGURATION) {
			
			if CHECK_COMMAND("comport", 0) {
				
				usart_write_line(USER_RS232, "\r\nParams: ");
				
				sprintf(ptemp, "%d", paramsCount);
				usart_write_line(USER_RS232, ptemp);
				
				for (i = 0; i < paramsCount; i++) {
					sprintf(ptemp, "\r\nParam %02d: ", i);
					usart_write_line(USER_RS232, ptemp);
					usart_write_line(USER_RS232, sub_str[i]);
				}
				
				recognized = TRUE;
				
			} else if (CHECK_COMMAND("exit", 0)  || CHECK_COMMAND("end", 0)) {
				usart_write_line(USER_RS232, "Saving changes...\r\n");
				/* TODO: SAVE */
				usart_write_line(USER_RS232, "Changes saved...\r\nRestarting unit...\r\n\r\n");
				delay_ms(50); /* �ek�n�, ne� se ode�le cel� �et�zec, na konci programu �ekac� smy�ka ni�emu nevad�... */
				reset_do_soft_reset();
			}
		}
		
		/*
		// Skryt� tov�rn� nastaven�
		if (statusMachine >= MACHINE_FACTORY_CONFIGURATION) {
			
		} */
		
		// P��kaz nerozpozn�n
		if (recognized == FALSE) {
			usart_write_line(USER_RS232, "Command not recognized\r\n");
			usart_write_line(USER_RS232, "Write 'help' to see the list of all commands\r\n");
		}
		
		//Uvoln�n� bufferu pro dal�� ��t�zec
		statusRS232 = RS232_INITIAL;
	}
}