/*
 * \file serialTask.c
 * \brief Separátní soubor pro funkci SerialTask
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
#include "serialTask.h"


static char *sub_str[10];				//pole ukazatelù rozdìlených podle mezer
static char s[2];						//pomocná promìnná pro rozsekání
static char toASCII[3];
static short int i, j;					//pomocná promìnná pro indexaci
static short int paramsCount;
static short int recognized = TRUE;		//pomocná promìnná pro vypsání nápovìdy o neznámém pøíkazu
static char ptemp[50];					//pomocná promìnná pro výpis

/* Variables for saving */
static uint32_t baud;
static uint16_t channelMask;
static uint8_t handShake;
static short int save_RS232_changes;
static uint8_t measTime;
static uint8_t PLfreq;
static uint8_t rounding;
static uint8_t science;
static char prefix[9];
static char separator[9];
static char suffix[9];
static char lineEnd[9];

void serialTask(void) {
	if (statusRS232 == RS232_READY_TO_PROCESS) {
		//rozøezání na podøetìzce podle mezery
		i = 0;
		s[0] = RS232_SEPARATOR; s[1] = 0x00;
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
			measTime			= publicConfig.measNPLC;
			PLfreq				= publicConfig.measPowerLineFreq;
			rounding			= publicConfig.measRounding;
			science				= publicConfig.measScientific;
			channelMask			= publicConfig.channelsToogleMask;
			strncpy(prefix,		publicConfig.outputPrefix,		8);
			strncpy(separator,	publicConfig.outputSeparator,	8);
			strncpy(suffix,		publicConfig.outputSuffix,		8);
			strncpy(lineEnd,	publicConfig.outputLineEnding,	8);
			prefix[8]		= '\0';
			separator[8]	= '\0';
			suffix[8]		= '\0';
			lineEnd[8]		= '\0';
			
			recognized = TRUE;

		}

		// Standartní konfiguraèní pøíkazy
		if (statusMachine >= MACHINE_USER_CONFIGURATION) {

			if CHECK_COMMAND("comport", 0) {

				if (paramsCount != 1) {
					i = 1;
					while (i < paramsCount)	{
						if CHECK_COMMAND("-b", i) {
							baud = atoi(sub_str[i + 1]);
							if ( (baud >= 1200) & (baud <= 115200) ) {
								sprintf(ptemp, "->New baud rate will be %d\r\n", baud);
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
								usart_write_line(USER_RS232, "->Hardware handshaking will be ON.\r\n");
								handShake = 1;
								i++;
								if (save_RS232_changes == UNKNOWN) {
									save_RS232_changes = FALSE;
								}
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Hardware handshaking will be OFF.\r\n");
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
				// END of COMPORT command
			
				
			} else if CHECK_COMMAND("meas", 0) {
				if (paramsCount != 1) {
					
					i = 1;
					while (i < paramsCount)	{
						if CHECK_COMMAND("-t", i) {
							measTime = atoi(sub_str[i + 1]);
							if ( (measTime >= 1) && (measTime <= 100) ) {
								sprintf(ptemp, "->New measurement time will be %d NPLC\r\n", measTime);
								usart_write_line(USER_RS232, ptemp);
								i++;
							} else {
								measTime = publicConfig.measNPLC;
								usart_write_line(USER_RS232, "Measurement time must be from 1 to 100.\r\n");
							}
						} //end if "-t"

						else if CHECK_COMMAND("-lf", i) {

							if CHECK_COMMAND("50", i + 1) {
								usart_write_line(USER_RS232, "->Power line frequency will be set to 50 Hz.\r\n");
								PLfreq = 50;
								i++;
								} else if CHECK_COMMAND("60", i + 1) {
								usart_write_line(USER_RS232, "->Power line frequency will be set to 60 Hz.\r\n");
								PLfreq = 60;
								i++;
								} else {
								usart_write_line(USER_RS232, "Power line frequency could be only 50 or 60 Hz.\r\n");
							}

						} //end if "-lf"

						else if CHECK_COMMAND("-rn", i) {

							if CHECK_COMMAND("on", i + 1) {
								usart_write_line(USER_RS232, "->Rounding will be enabled.\r\n");
								rounding = 1;
								i++;
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Rounding will be disabled.\r\n");
								rounding = 0;
								i++;
							} else {
								usart_write_line(USER_RS232, "Rounding could be only on or off.\r\n");
							}

						} //end if "-rn"

						else if CHECK_COMMAND("-sc", i) {

							if CHECK_COMMAND("on", i + 1) {
								usart_write_line(USER_RS232, "->Scientific notation will be enabled.\r\n");
								science = 1;
								i++;
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Scientific notation will be disabled.\r\n");
								science = 0;
								i++;
							} else {
								usart_write_line(USER_RS232, "Scientific notation could be only on or off.\r\n");
							}

						} //end if "-sc"
						
						i++;
					} // end-while through params

					usart_write_line(USER_RS232, "The results will be sent out via RS-232 port in format:\r\n");
					outputStringExample( prefix, separator, suffix, lineEnd, rounding, science );
					usart_write_line(USER_RS232, "\r\n\r\n");
					measTimeInfo( measTime, PLfreq, hiddenConfig.settlingTime, channelMask, baud );
					
					} else {
					showMeasHelp();
				}
					
				recognized = TRUE;
				// END of MEAS command
				
			} else if CHECK_COMMAND("output", 0) {
				if (paramsCount != 1) {

					i = 1;
					while (i < paramsCount)	{

						if CHECK_COMMAND("-p", i) {

							if (sub_str[i+1][0] == '"') {
								j = 0;
								while ( (sub_str[i+1][++j] != '"') && (sub_str[i+1][j] != '\0') && (j < 9) )
									prefix[j-1] = sub_str[i+1][j];
								prefix[j-1] = '\0';
								usart_write_line(USER_RS232, "New prefix entered\r\n");
								i++;
							} else {
								usart_write_line(USER_RS232, "Parameter must start with \" sign \r\n");
							}

						} //end if "-p"

						else if CHECK_COMMAND("-pa", i) {

							j = 0;
							while ( (sub_str[i+1][2*j] != '\0') && (j < 18) ){
								toASCII[0] = sub_str[i+1][2*j];
								toASCII[1] = sub_str[i+1][2*j+1];
								toASCII[2] = '\n';
								prefix[j] = (char)strtoul(toASCII, NULL, 16);
								j++;
							}
							prefix[j] = '\0';
							usart_write_line(USER_RS232, "New prefix entered\r\n");

							i++;
						} //end if "-pa"

						else if CHECK_COMMAND("-s", i) {

							if (sub_str[i+1][0] == '"') {
								j = 0;
								while ( (sub_str[i+1][++j] != '"') && (sub_str[i+1][j] != '\0') && (j < 9) )
									separator[j-1] = sub_str[i+1][j];
								separator[j-1] = '\0';
								usart_write_line(USER_RS232, "New separator entered\r\n");
								i++;
							} else {
								usart_write_line(USER_RS232, "Parameter must start with \" sign \r\n");
							}

						} //end if "-s"
						
						else if CHECK_COMMAND("-sa", i) {

							j = 0;
							while ( (sub_str[i+1][2*j] != '\0') && (j < 18) ){
								toASCII[0] = sub_str[i+1][2*j];
								toASCII[1] = sub_str[i+1][2*j+1];
								toASCII[2] = '\n';
								separator[j] = (char)strtoul(toASCII, NULL, 16);
								j++;
							}
							separator[j] = '\0';
							usart_write_line(USER_RS232, "New separator entered\r\n");

							i++;
						} //end if "-sa"
					
						else if CHECK_COMMAND("-u", i) {

							if (sub_str[i+1][0] == '"') {
								j = 0;
								while ( (sub_str[i+1][++j] != '"') && (sub_str[i+1][j] != '\0') && (j < 9) )
									suffix[j-1] = sub_str[i+1][j];
								suffix[j-1] = '\0';
								usart_write_line(USER_RS232, "New suffix entered\r\n");
								i++;
							} else {
								usart_write_line(USER_RS232, "Parameter must start with \" sign \r\n");
							}

						} //end if "-u"
						
						else if CHECK_COMMAND("-ua", i) {

							j = 0;
							while ( (sub_str[i+1][2*j] != '\0') && (j < 18) ){
								toASCII[0] = sub_str[i+1][2*j];
								toASCII[1] = sub_str[i+1][2*j+1];
								toASCII[2] = '\n';
								suffix[j] = (char)strtoul(toASCII, NULL, 16);
								j++;
							}
							suffix[j] = '\0';
							usart_write_line(USER_RS232, "New suffix entered\r\n");

							i++;
						} //end if "-ua"

						else if CHECK_COMMAND("-l", i) {

							if CHECK_COMMAND("None", i + 1) {
								lineEnd[0] = '\0';
								usart_write_line(USER_RS232, "Line ending set to None.\r\n");
								i++;
							} else if (CHECK_COMMAND("ETX", i + 1) || CHECK_COMMAND("etx", i + 1)) {
								lineEnd[0] = 0x03;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to ETX (End of Text).\r\n");
								i++;
							} else if (CHECK_COMMAND("EOT", i + 1) || CHECK_COMMAND("eot", i + 1)) {
								lineEnd[0] = 0x04;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to EOT (End of Transmission).\r\n");
								i++;
							} else if (CHECK_COMMAND("CR", i + 1) || CHECK_COMMAND("cr", i + 1)) {
								lineEnd[0] = 0x0d;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to CR (Carriage Return).\r\n");
								i++;
							} else if (CHECK_COMMAND("LF", i + 1) || CHECK_COMMAND("lf", i + 1)){
								lineEnd[0] = 0x0a;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to LF (Line Feed).\r\n");
								i++;
							} else if (CHECK_COMMAND("CRLF", i + 1) || CHECK_COMMAND("crlf", i + 1)) {
								lineEnd[0] = 0x0d;
								lineEnd[1] = 0x0a;
								lineEnd[2] = '\0';
								usart_write_line(USER_RS232, "Line ending set to CRLF (Carriage Return and Line Feed).\r\n");
								i++;
							} else if (CHECK_COMMAND("LFCR", i + 1) || CHECK_COMMAND("lfcr", i + 1)) {
								lineEnd[0] = 0x0a;
								lineEnd[1] = 0x0d;
								lineEnd[2] = '\0';
								usart_write_line(USER_RS232, "Line ending set to LFCR (Line Feed and Carriage Return).\r\n");
								i++;
							} else {
								usart_write_line(USER_RS232, "Not valid line ending.\r\n");
							}

						} //end if "-l"

						else if CHECK_COMMAND("-la", i) {

							j = 0;
							while ( (sub_str[i+1][2*j] != '\0') && (j < 18) ){
								toASCII[0] = sub_str[i+1][2*j];
								toASCII[1] = sub_str[i+1][2*j+1];
								toASCII[2] = '\n';
								lineEnd[j] = (char)strtoul(toASCII, NULL, 16);
								j++;
							}
							lineEnd[j] = '\0';
							usart_write_line(USER_RS232, "New line ending entered\r\n");

							i++;
						} //end if "-ua"

						i++;
					} // end-while through params

					usart_write_line(USER_RS232, "The results will be sent out via RS-232 port in format:\r\n");
					outputStringExample( prefix, separator, suffix, lineEnd, rounding, science );
					usart_write_line(USER_RS232, "\r\n\r\n");

					} else {
					showOutputHelp();
				}
			
				recognized = TRUE;
				// END of OUTPUT command

			} else if (CHECK_COMMAND("exit", 0)  || CHECK_COMMAND("end", 0)) {
				usart_write_line(USER_RS232, "Saving changes...\r\n");
				/* TODO: SAVE */

				if ( strncmp(prefix, publicConfig.outputPrefix, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputPrefix, &prefix, sizeof(publicConfig.outputPrefix), TRUE);
					usart_write_line(USER_RS232, "->Prefix set to '");
					usart_write_line(USER_RS232, prefix);
					usart_write_line(USER_RS232, "'\r\n");
				}
				
				if ( strncmp(separator, publicConfig.outputSeparator, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputSeparator, &separator, sizeof(publicConfig.outputSeparator), TRUE);
					usart_write_line(USER_RS232, "->Separator set to '");
					usart_write_line(USER_RS232, separator);
					usart_write_line(USER_RS232, "'\r\n");
				}
				
				if ( strncmp(suffix, publicConfig.outputSuffix, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputSuffix, &suffix, sizeof(publicConfig.outputSuffix), TRUE);
					usart_write_line(USER_RS232, "->Suffix set to '");
					usart_write_line(USER_RS232, suffix);
					usart_write_line(USER_RS232, "'\r\n");
				}
				
				if ( strncmp(lineEnd, publicConfig.outputLineEnding, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputLineEnding, &lineEnd, sizeof(publicConfig.outputLineEnding), TRUE);
					usart_write_line(USER_RS232, "->New line ending set\r\n");
				}
								
				if (measTime != publicConfig.measNPLC) {
					flashc_memcpy(&publicConfig.measNPLC, &measTime, sizeof(measTime), TRUE);
					sprintf(ptemp, "->Measurement time set to %2d\r\n", measTime);
					usart_write_line(USER_RS232, ptemp);
				}
				
				if (PLfreq != publicConfig.measPowerLineFreq) {
					flashc_memcpy(&publicConfig.measPowerLineFreq, &PLfreq, sizeof(PLfreq), TRUE);
					sprintf(ptemp, "->Power line frequency set to %2d\r\n", PLfreq);
					usart_write_line(USER_RS232, ptemp);
				}
				
				if (rounding != publicConfig.measRounding) {
					flashc_memcpy(&publicConfig.measRounding, &rounding, sizeof(rounding), TRUE);
					usart_write_line(USER_RS232, "->Rounding set to ");
					if (rounding == 1) {
						usart_write_line(USER_RS232, "ON\r\n");
					} 
					else {
						usart_write_line(USER_RS232, "OFF\r\n");
					}
				}
				
				if (science != publicConfig.measScientific) {
					flashc_memcpy(&publicConfig.measScientific, &science, sizeof(science), TRUE);
					usart_write_line(USER_RS232, "->Scientific notation set to ");
					if (science == 1) {
						usart_write_line(USER_RS232, "ON\r\n");
					} 
					else {
						usart_write_line(USER_RS232, "OFF\r\n");
					}
				}
				

				if (save_RS232_changes == TRUE) {
					
					if (baud != publicConfig.comPortBaudrate) {
						flashc_memcpy(&publicConfig.comPortBaudrate, &baud, sizeof(baud), TRUE);
						sprintf(ptemp, "->Baud rate set to %2d\r\n", baud);
						usart_write_line(USER_RS232, ptemp);
					}
					
					if (handShake != publicConfig.comPortHandshake) {
						flashc_memcpy(&publicConfig.comPortHandshake, &handShake, sizeof(handShake), TRUE);
						usart_write_line(USER_RS232, "->RTS/CTS set to ");
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
#ifdef DEBUG
			usart_write_line(USER_RS232, sub_str[0]);
#endif //DEBUG
		}
		
		//Uvolnìní bufferu pro další øìtìzec
		statusRS232 = RS232_INITIAL;
	}
}