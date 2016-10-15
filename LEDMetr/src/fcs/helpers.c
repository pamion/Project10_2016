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

static char *sub_str[10];				//pole ukazatelù rozdìlených podle mezer
static const char s[2] = " ";			//pomocná promìnná pro rozsekání
static short int i;						//pomocná promìnná pro indexaci
static short int paramsCount;
static short int recognized = TRUE;		//pomocná promìnná pro vypsání nápovìdy o neznámém pøíkazu
static char ptemp[20];					//pomocná promìnná pro výpis

/* Variables for saving */
static uint16_t baud;
static uint8_t handShake;
static short int save_RS232_changes;

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
		//rozøezání na podøetìzce podle mezery
		i = 0;
		sub_str[i] = strtok(bufferRS232, s);
		while( sub_str[i] != NULL ) {
			sub_str[++i] = strtok(NULL, s);
		}

		//Odstranìní øídících znakù, když se objeví (pro jistotu)
		i = 0;
		while( sub_str[i++] != NULL ) {
			//Odstraníme znak nového øádku, pokud by se nám dostal na konec pøíkazu
			sub_str[i][strcspn ( sub_str[i], "\n" )] = '\0';
			//Odstraníme znak "carriage return", pokud by se nám dostal na konec pøíkazu
			sub_str[i][strcspn ( sub_str[i], "\r" )] = '\0';
		}

		paramsCount = i - 1; 
		recognized = FALSE;

		//Vykonání základních pøíkazù
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

			// Initialization of temp variables
			baud				= publicConfig.comPortBaudrate;
			handShake			= publicConfig.comPortHandshake;
			save_RS232_changes	= UNKNOWN;
					
			recognized = TRUE;

		}

		// Standartní konfiguraèní pøíkazy
		if (statusMachine >= MACHINE_USER_CONFIGURATION) {

			if CHECK_COMMAND("comport", 0) {
				/*
				usart_write_line(USER_RS232, "\r\nParams: ");

				sprintf(ptemp, "%d", paramsCount);
				usart_write_line(USER_RS232, ptemp);

				for (i = 0; i < paramsCount; i++) {
					sprintf(ptemp, "\r\nParam %02d: ", i);
					usart_write_line(USER_RS232, ptemp);
					usart_write_line(USER_RS232, sub_str[i]);
				}
				usart_write_line(USER_RS232, "\r\n\r\n");
				*/

				if (paramsCount != 1) {
					i = 1;
					while (i < paramsCount)	{
						if CHECK_COMMAND("-b", i) {
							baud = atoi(sub_str[i + 1]);
							if ( (baud > 1200) && (baud < 115200) ) {
								usart_write_line(USER_RS232, ptemp);
								i++;
								if (save_RS232_changes == UNKNOWN) {
									save_RS232_changes = FALSE;
								}
							} else {
								baud = publicConfig.comPortBaudrate;
								usart_write_line(USER_RS232, "Baud rate must be between 1200 and 115200 baud.\r\n");
							}
						} //end if "-b"

						else if CHECK_COMMAND("-h", i) {

							if CHECK_COMMAND("on", i + 1) {
								handShake = 1;
								i++;
								if (save_RS232_changes == UNKNOWN) {
									save_RS232_changes = FALSE;
								}
							} else if CHECK_COMMAND("off", i + 1) {
								handShake = 0;
								i++;
								if (save_RS232_changes == UNKNOWN) {
									save_RS232_changes = FALSE;
								}
							} else {
								usart_write_line(USER_RS232, "Hardware handshaking could be only on or off.\r\n");
							}

						} //end if "-h"
						else if CHECK_COMMAND("-s", i) {
							save_RS232_changes = TRUE;
							usart_write_line(USER_RS232, "Changes will be saved\r\n");
						}
						i++;
					} // end-while through params

					if (save_RS232_changes == FALSE) {
						showComportWarning();
						bufferRS232[0] = 0x00;
						statusRS232 = RS232_WAIT_2_CONFIRM;
						while ( (bufferRS232[0] != 'Y') && (bufferRS232[0] != 'y') && (bufferRS232[0] != 'N') && (bufferRS232[0] != 'n') ) { 
						}
						if ( (bufferRS232[0] == 'Y') || (bufferRS232[0] == 'y') ) {
							save_RS232_changes = TRUE;
							usart_write_line(USER_RS232, "Changes will be saved\r\n");
						} else {
							save_RS232_changes = FALSE;
							usart_write_line(USER_RS232, "Changes won't be saved\r\n");
						}
					}

				} else {
					showComportHelp();
				}
				recognized = TRUE;

			} else if (CHECK_COMMAND("exit", 0)  || CHECK_COMMAND("end", 0)) {
				usart_write_line(USER_RS232, "Saving changes...\r\n");
				/* TODO: SAVE */

				if (save_RS232_changes == TRUE) {
					
					if (baud != publicConfig.comPortBaudrate) {
						flashc_memcpy(&publicConfig.comPortBaudrate, &baud, sizeof(baud), TRUE);
						usart_write_line(USER_RS232, "->Baud rate saved\r\n");
						sprintf(ptemp, "->New baud rate will be %2d\r\n", baud);
						usart_write_line(USER_RS232, ptemp);
					}
					
					if (handShake != publicConfig.comPortHandshake) {
						flashc_memcpy(&publicConfig.comPortHandshake, &handShake, sizeof(handShake), TRUE);
						usart_write_line(USER_RS232, "->Handshake saved\r\n->RTS/CTS is ");
						if (handShake == 1) {
							usart_write_line(USER_RS232, "ON\r\n");
						} 
						else {
							usart_write_line(USER_RS232, "OFF\r\n");
						}
						
					}
				} // if (save_RS232_changes == TRUE)

				usart_write_line(USER_RS232, "Changes saved...\r\nRestarting unit...\r\n\r\n");
				delay_ms(50); /* Èekání, než se odešle celý øetìzec, na konci programu èekací smyèka nièemu nevadí... */
				reset_do_soft_reset();
			}
		}

		/*
		// Skrytá tovární nastavení
		if (statusMachine >= MACHINE_FACTORY_CONFIGURATION) {
			
		} */
		
		// Pøíkaz nerozpoznán
		if (recognized == FALSE) {
			usart_write_line(USER_RS232, "Command not recognized\r\n");
			usart_write_line(USER_RS232, "Write 'help' to see the list of all commands\r\n");
		}
		
		//Uvolnìní bufferu pro další øìtìzec
		statusRS232 = RS232_INITIAL;
	}
}