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
#include "buffer.h"
#include "serialTask.h"

static char subBuff[SUBBUFF_LEN][20];	//pole ukazatelù rozdìlených podle mezer
/*static char s[2];						//pomocná promìnná pro rozsekání
static char toASCII[3];*/
static short int i, j;					//pomocná promìnná pro indexaci
static short int paramsCount;

static short int recognized = TRUE;		//pomocná promìnná pro vypsání nápovìdy o neznámém pøíkazu
static short int badArguments = FALSE;	
static char ptemp[50];					//pomocná promìnná pro výpis
static short int valOut;


/* Variables for saving */
nvram_data_t1 hiddenConfigNew, hiddenConfig2Save;
nvram_data_t2 publicConfigNew, publicConfig2Save;
/*
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
*/
void serialTask(void)
{
	
	int buff_stat = BUFFER_INIT;

	if ( bufferIsLineReady(&buffIn) ) {
		i = 0;
		recognized = TRUE;
		badArguments = FALSE;
		//Vyètení øádku z bufferu
		buff_stat = BUFFER_SUCCESS;
		while (buff_stat == BUFFER_SUCCESS) {
			buff_stat = bufferReadWord(&buffIn, &subBuff[i++]);
		}
		paramsCount = i;
		for (i; i < SUBBUFF_LEN; i++) {
			subBuff[i++][0] = NULL;
		}
		
		//Vykonání základních pøíkazù
		if CHECK_COMMAND("help", 0) {
			usart_write_line(USER_RS232, "Showing help\r\n");

		} else if CHECK_COMMAND("info", 0) {
			showInfoText();

		} else if CHECK_COMMAND("config", 0) {
			if CHECK_COMMAND(CONFIG_PASS, 1) {
				statusMachine = MACHINE_FACTORY_CONFIGURATION;
				usart_write_line(USER_RS232, "HIDDEN FACTORY SETTINGS:\r\n");
			} else {
				statusMachine = MACHINE_USER_CONFIGURATION;
				showConfigText();
			}
			
			/* Prepare global variables for saving
			   Two separate instances are needed for algorithm to be capable roll back changes after unsuccesfull command */	
			memcpy(&hiddenConfigNew, &hiddenConfig, sizeof(hiddenConfig));
			memcpy(&hiddenConfig2Save, &hiddenConfig, sizeof(hiddenConfig));
			memcpy(&publicConfigNew, &publicConfig, sizeof(hiddenConfig));
			memcpy(&publicConfig2Save, &publicConfig, sizeof(hiddenConfig));
		} else {
			recognized = FALSE;
		}
		
		// Standartní konfiguraèní pøíkazy
		if ( (statusMachine >= MACHINE_USER_CONFIGURATION) && (recognized == FALSE) ) {
			recognized = TRUE;
			
			if CHECK_COMMAND("comport", 0) {
				usart_write_line(USER_RS232, "Comport command: Comming soon...\r\n");
				
				//End of COMPORT command
				
			} else if CHECK_COMMAND("output", 0) {
				usart_write_line(USER_RS232, "Output command: Comming soon...\r\n");
				
				//End of OUTPUT command
				
			} else if CHECK_COMMAND("meas", 0) {
				usart_write_line(USER_RS232, "Meas command: Comming soon...\r\n");
				
				//End of MEAS command
				
			} else if CHECK_COMMAND("channels", 0) {
				usart_write_line(USER_RS232, "Channels command: Comming soon...\r\n");
				
				//End of CHANNELS command
				
			} else if CHECK_COMMAND("defaults", 0) {
				usart_write_line(USER_RS232, "Defaults command: Comming soon...\r\n");
				
				//End of DEFAULTS command
				
			} else if CHECK_COMMAND("help", 0) {
				usart_write_line(USER_RS232, "Help command: Comming soon...\r\n");
				
				//End of HELP command
				
			} else if CHECK_COMMAND("info", 0) {
				usart_write_line(USER_RS232, "Info command: Comming soon...\r\n");
				
				//End of INFO command
			
			} else if CHECK_COMMAND("expconf", 0) {
				usart_write_line(USER_RS232, "Expconf command: Comming soon...\r\n");
				
				//End of EXPCONF command
					
			} else if CHECK_COMMAND("discart", 0) {
				usart_write_line(USER_RS232, "Discarting changes...\r\n");
				usart_write_line(USER_RS232, "Restarting the luxmeter...\r\n\r\n");
				delay_ms(50); // Èekání, než se odešle celý øetìzec, na konci programu èekací smyèka nièemu nevadí...
				reset_do_soft_reset();
				// END of DISCART command
			
			} else if ( CHECK_COMMAND("exit", 0) || CHECK_COMMAND("end", 0) || CHECK_COMMAND("save", 0) ) {
				usart_write_line(USER_RS232, "Saving changes...\r\n");
				
				/*TODO: SAVE */
				
				
				usart_write_line(USER_RS232, "Restarting the luxmeter...\r\n\r\n");
				delay_ms(50); // Èekání, než se odešle celý øetìzec, na konci programu èekací smyèka nièemu nevadí...
				reset_do_soft_reset();
				// END of END/EXIT commands
				
			} else {
				recognized = FALSE;
			}
		} // END of standart config. commands
		

		// Skrytá tovární nastavení
		if ( (statusMachine >= MACHINE_FACTORY_CONFIGURATION) && (recognized == FALSE) ) {
			recognized = TRUE;
			if CHECK_COMMAND("hwver", 0) {
				usart_write_line(USER_RS232, "HW version command: Comming soon...\r\n");
				
				//End of HWVER command
				
			} else if CHECK_COMMAND("sernum", 0) {
				usart_write_line(USER_RS232, "Serial number change command: Comming soon...\r\n");
				
				//End of SERNUM command
				
			} else if CHECK_COMMAND("adcparam", 0) {
				usart_write_line(USER_RS232, "A/D converter parameters command: Comming soon...\r\n");
				
				//End of ADCPARAMS command
							
			} else if CHECK_COMMAND("pdsens", 0) {
				usart_write_line(USER_RS232, "PhotoDiode Sensitivity command: Comming soon...\r\n");
				
				//End of PDSENS command
				
			} else if CHECK_COMMAND("calib", 0) {
				usart_write_line(USER_RS232, "Calibration command: Comming soon...\r\n");
				
				//End of CALIB command			
						
			} else {
				recognized = FALSE;
			}			
		} // END of factory config. commands

		// Pøíkaz nerozpoznán - bìhem konfigurace - nepøišel znak enter
		if ( (recognized == FALSE) && (statusMachine >= MACHINE_USER_CONFIGURATION) && (subBuff[0][0] != NULL) ) {
			usart_write_line(USER_RS232, "Error: Bad command or argument\r\n");
			usart_write_line(USER_RS232, "Enter \"help\" to get list of available commands.\r\n");
#ifdef DEBUG
			usart_write_line(USER_RS232, "\r\nDebug info: \r\n");
			for ( i=0; i < paramsCount; i++ ) {
				usart_write_line(USER_RS232, subBuff[i]);
				usart_write_line(USER_RS232, " ");
			}
#endif //DEBUG
		}		
		
		/* Show call sign after each commands */


		if ( ( buff_stat == BUFFER_LINE_END ) && ( statusMachine != MACHINE_MEASURE ) ) {
			if (statusMachine >= MACHINE_FACTORY_CONFIGURATION) {
				usart_write_line(USER_RS232, CMD_LINE_SIGN_NORMAL);
			} else {
				usart_write_line(USER_RS232, CMD_LINE_SIGN_NORMAL);
			}
		}
		
	} /* END main IF - if buffer is ready */

	
	

	
	/*
	int buff_stat;
	
	if ( bufferIsLineReady(&buffIn) ) {
		i = 0;
		buff_stat = BUFFER_SUCCESS;
		while (buff_stat == BUFFER_SUCCESS) {
			buff_stat = bufferReadWord(&buffIn, &subBuff[i++]);
		}

		paramsCount = i - 1; 
		recognized = FALSE;
		badArguments = FALSE;

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
			badArguments = FALSE;
		}

		// Standartní konfiguraèní pøíkazy
		if (statusMachine >= MACHINE_USER_CONFIGURATION) {

			if CHECK_COMMAND("comport", 0) {

				if (paramsCount != 1) {
					i = 1;
					while (i < paramsCount)	{
						if CHECK_COMMAND("-b", i) {
							if (validateInput(subBuff[i+1], VAL_INTEGER)) {
								baud = atoi(subBuff[i + 1]);
								if ( (baud >= 1200) & (baud <= 115200) ) {
									sprintf(ptemp, "->New baud rate will be %d\r\n", baud);
									usart_write_line(USER_RS232, ptemp);
									if (save_RS232_changes == UNKNOWN) {
										save_RS232_changes = FALSE;
									}
								} else {
									baud = publicConfig.comPortBaudrate;
									usart_write_line(USER_RS232, "Error: Baud rate must be between 1200 and 115200 baud.\r\n");
									badArguments = TRUE;
								}
							} else {
								baud = publicConfig.comPortBaudrate;
								usart_write_line(USER_RS232, "Error: Baud rate must contain only numbers.\r\n");
								badArguments = TRUE;
							}
							i++;
						} //end if "-b"

						else if CHECK_COMMAND("-h", i) {

							if CHECK_COMMAND("on", i + 1) {
								usart_write_line(USER_RS232, "->Hardware handshaking will be ON.\r\n");
								handShake = 1;
								if (save_RS232_changes == UNKNOWN) {
									save_RS232_changes = FALSE;
								}
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Hardware handshaking will be OFF.\r\n");
								handShake = 0;
								if (save_RS232_changes == UNKNOWN) {
									save_RS232_changes = FALSE;
								}
							} else {
								usart_write_line(USER_RS232, "Error: Hardware handshaking could be only on or off.\r\n");
								badArguments = TRUE;
							}
							i++;
						} //end if "-h"
						else if CHECK_COMMAND("-s", i) {
							save_RS232_changes = TRUE;
							usart_write_line(USER_RS232, "Changes will be saved\r\n");
						}
						
						else {
							if (badArguments == FALSE) {
								usart_write_line(USER_RS232, "Error: Invalid argument\r\n");
							}
							badArguments = TRUE;
							save_RS232_changes = UNKNOWN;
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
							usart_write_line(USER_RS232, "\r\n");
						}
					}

				} else {
					showComportHelp();
				}
				recognized = TRUE;
				// END of COMPORT command
			
				
			} else if CHECK_COMMAND("meas", 0) {

				if (paramsCount != 1) {
					badArguments = FALSE;
					i = 1;
					while (i < paramsCount)	{
						if CHECK_COMMAND("-t", i) {
							if (validateInput(subBuff[i+1], VAL_INTEGER)) {
								measTime = atoi(subBuff[i+1]);
								if ( (measTime >= 1) && (measTime <= 100) ) {
									sprintf(ptemp, "->New measurement time will be %d NPLC\r\n", measTime);
									usart_write_line(USER_RS232, ptemp);
								} else {
									measTime = publicConfig.measNPLC;
									usart_write_line(USER_RS232, "Error: Measurement time can be set between 1 and 100 NPLCs.\r\n");
									badArguments = TRUE;
								}
							} else {
								measTime = publicConfig.measNPLC;
								usart_write_line(USER_RS232, "Error: Measurement time can be integer only.\r\n");
								badArguments = TRUE;
							}
							i++;
						} //end if "-t"

						else if CHECK_COMMAND("-lf", i) {

							if CHECK_COMMAND("50", i + 1) {
								usart_write_line(USER_RS232, "->Power line frequency will be set to 50 Hz.\r\n");
								PLfreq = 50;
							} else if CHECK_COMMAND("60", i + 1) {
								usart_write_line(USER_RS232, "->Power line frequency will be set to 60 Hz.\r\n");
								PLfreq = 60;
							} else {
								usart_write_line(USER_RS232, "Error: Power line frequency could be only 50 or 60 Hz.\r\n");
								badArguments = TRUE;
							}
							i++;

						} //end if "-lf"

						else if CHECK_COMMAND("-rn", i) {

							if CHECK_COMMAND("on", i + 1) {
								usart_write_line(USER_RS232, "->Rounding will be enabled.\r\n");
								rounding = 1;
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Rounding will be disabled.\r\n");
								rounding = 0;
							} else {
								usart_write_line(USER_RS232, "Error: Rounding could be only on or off.\r\n");
								badArguments = TRUE;
							}
							i++;
						} //end if "-rn"

						else if CHECK_COMMAND("-sc", i) {

							if CHECK_COMMAND("on", i + 1) {
								usart_write_line(USER_RS232, "->Scientific notation will be enabled.\r\n");
								science = 1;
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Scientific notation will be disabled.\r\n");
								science = 0;
							} else {
								usart_write_line(USER_RS232, "Error: Scientific notation could be only on or off.\r\n");
								badArguments = TRUE;
							}
							i++;

						} //end if "-sc"
						
						else {
							if (badArguments == FALSE) {
								badArguments = TRUE;
								usart_write_line(USER_RS232, "Error: Invalid argument\r\n");
							}
						}
						i++;
					} // end-while through params
					
					if (badArguments == FALSE) {
						usart_write_line(USER_RS232, "The results will be sent out via RS-232 port in format:\r\n");
						outputStringExample( prefix, separator, suffix, lineEnd, rounding, science );
						usart_write_line(USER_RS232, "\r\n\r\n");
						measTimeInfo( measTime, PLfreq, hiddenConfig.settlingTime, channelMask, baud );
					}
					
					
				} else {
					showMeasHelp();
				}
					
				recognized = TRUE;
				// END of MEAS command
				
			} else if CHECK_COMMAND("output", 0) {
				if (paramsCount != 1) {

					i = 1;
					badArguments = FALSE;
					valOut = 0;
					while (i < paramsCount)	{

						if CHECK_COMMAND("-p", i) {
							valOut = validateInput( subBuff[i+1], VAL_OUTPUT_STR );
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][++j] != '"') && (subBuff[i+1][j] != '\0') && (j < 9) )
									prefix[j-1] = subBuff[i+1][j];
								prefix[j-1] = '\0';
								usart_write_line(USER_RS232, "New prefix entered\r\n");
							} else {
								badArguments = TRUE;
							}
							i++;
						} //end if "-p"

						else if CHECK_COMMAND("-pa", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									prefix[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								prefix[j] = '\0';
								usart_write_line(USER_RS232, "New prefix entered\r\n");
							} else {
								badArguments = TRUE;
							}
							i++;
						} //end if "-pa"

						else if CHECK_COMMAND("-s", i) {
							valOut = validateInput( subBuff[i+1], VAL_OUTPUT_STR );
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][++j] != '"') && (subBuff[i+1][j] != '\0') && (j < 9) )
									separator[j-1] = subBuff[i+1][j];
								separator[j-1] = '\0';
								usart_write_line(USER_RS232, "New separator entered\r\n");
							} else {
								badArguments = TRUE;
							}
							i++;
						} //end if "-s"
						
						else if CHECK_COMMAND("-sa", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									separator[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								separator[j] = '\0';
								usart_write_line(USER_RS232, "New separator entered\r\n");
							} else {
								badArguments = TRUE;
							}
							i++;
						} //end if "-sa"
					
						else if CHECK_COMMAND("-u", i) {
							valOut = validateInput( subBuff[i+1], VAL_OUTPUT_STR );
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][++j] != '"') && (subBuff[i+1][j] != '\0') && (j < 9) )
									suffix[j-1] = subBuff[i+1][j];
								suffix[j-1] = '\0';
								usart_write_line(USER_RS232, "New suffix entered\r\n");
							} else {
								badArguments = TRUE;
							}
							i++;
						} //end if "-u"
						
						else if CHECK_COMMAND("-ua", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									suffix[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								suffix[j] = '\0';
								usart_write_line(USER_RS232, "New suffix entered\r\n");
							} else {
								badArguments = TRUE;
							}
							i++;
						} //end if "-ua"

						else if CHECK_COMMAND("-l", i) {
							if CHECK_COMMAND("None", i + 1) {
								lineEnd[0] = '\0';
								usart_write_line(USER_RS232, "Line ending set to None.\r\n");
							} else if (CHECK_COMMAND("ETX", i + 1) || CHECK_COMMAND("etx", i + 1)) {
								lineEnd[0] = 0x03;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to ETX (End of Text).\r\n");
							} else if (CHECK_COMMAND("EOT", i + 1) || CHECK_COMMAND("eot", i + 1)) {
								lineEnd[0] = 0x04;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to EOT (End of Transmission).\r\n");
							} else if (CHECK_COMMAND("CR", i + 1) || CHECK_COMMAND("cr", i + 1)) {
								lineEnd[0] = 0x0d;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to CR (Carriage Return).\r\n");
							} else if (CHECK_COMMAND("LF", i + 1) || CHECK_COMMAND("lf", i + 1)){
								lineEnd[0] = 0x0a;
								lineEnd[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to LF (Line Feed).\r\n");
							} else if (CHECK_COMMAND("CRLF", i + 1) || CHECK_COMMAND("crlf", i + 1)) {
								lineEnd[0] = 0x0d;
								lineEnd[1] = 0x0a;
								lineEnd[2] = '\0';
								usart_write_line(USER_RS232, "Line ending set to CRLF (Carriage Return and Line Feed).\r\n");
							} else if (CHECK_COMMAND("LFCR", i + 1) || CHECK_COMMAND("lfcr", i + 1)) {
								lineEnd[0] = 0x0a;
								lineEnd[1] = 0x0d;
								lineEnd[2] = '\0';
								usart_write_line(USER_RS232, "Line ending set to LFCR (Line Feed and Carriage Return).\r\n");
							} else {
								usart_write_line(USER_RS232, "Error: Not valid line ending.\r\n");
								badArguments = TRUE;
							}
							i++;
						} //end if "-l"

						else if CHECK_COMMAND("-la", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									lineEnd[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								lineEnd[j] = '\0';
								usart_write_line(USER_RS232, "New line ending entered\r\n");
							} else {
								badArguments = TRUE;
							}
							i++;					
						} //end if "-ua"
						
						else {
							if (badArguments == FALSE) {
								badArguments = TRUE;
								usart_write_line(USER_RS232, "Error: Invalid argument\r\n");
							}
						}
					
						if ( valOut == VAL_HEX_LEN_ERR ) {
							usart_write_line(USER_RS232, "Error: Hexadecimal ASCII inputs are limited to 8 characters (16 HEX chars).\r\n");
						} else if ( valOut == VAL_STR_LEN_ERR ) {
							usart_write_line(USER_RS232, "Error: String input limited to 8 characters.\r\n");
						} else if ( valOut == VAL_STR_QOTATION_ERR ) {
							usart_write_line(USER_RS232, "Error: Parameter must be enclosed inside quotation marks, e.g. \":\"\r\n");
						} else if ( valOut == VAL_ODD_CHARS ) {
							usart_write_line(USER_RS232, "Error: Hexadecimal ASCII input must have even number of characters.\r\n");
						} else if ( valOut == VAL_BAD_HEX ) {
							usart_write_line(USER_RS232, "Error: Invalid ASCII code. Hexadecimal ASCII input is required.\r\n");
						}

						i++;
					} // end-while through params

					if ( badArguments == FALSE ) {
						usart_write_line(USER_RS232, "The results will be sent out via RS-232 port in format:\r\n");
						outputStringExample( prefix, separator, suffix, lineEnd, rounding, science );
						usart_write_line(USER_RS232, "\r\n\r\n");
					}

				} else {
					showOutputHelp();
				}
			
				recognized = TRUE;
				// END of OUTPUT command

			} else if (CHECK_COMMAND("exit", 0)  || CHECK_COMMAND("end", 0)) {
				usart_write_line(USER_RS232, "Saving changes...\r\n");
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

				usart_write_line(USER_RS232, "Restarting the luxmeter...\r\n\r\n");
				delay_ms(50); // Èekání, než se odešle celý øetìzec, na konci programu èekací smyèka nièemu nevadí... 
				reset_do_soft_reset();
				// END of EXIT command
			} 
		}
		*/

		/*
		// Skrytá tovární nastavení
		if (statusMachine >= MACHINE_FACTORY_CONFIGURATION) {
			
		} */
		
		/*
		// Pøíkaz nerozpoznán
		if (recognized == FALSE) {
			usart_write_line(USER_RS232, "Error: Bad command or argument\r\n");
			usart_write_line(USER_RS232, "Enter \"help\" to get list of available commands.\r\n");
#ifdef DEBUG
			usart_write_line(USER_RS232, "\r\nDebug info: \r\n");
			for ( i=0; i < paramsCount; i++ ) {
				usart_write_line(USER_RS232, subBuff[i]);
				usart_write_line(USER_RS232, " ");
			}
			usart_write_line(USER_RS232, "\r\n\r\n");
#endif //DEBUG
		}
		
		//show command line call sign
		if (statusMachine >= MACHINE_USER_CONFIGURATION) {
			usart_write_line(USER_RS232, CMD_LINE_CALL_SIGN);
		}
		
		
		//Uvolnìní bufferu pro další øìtìzec
		statusRS232 = RS232_INITIAL;
	}
	*/
}