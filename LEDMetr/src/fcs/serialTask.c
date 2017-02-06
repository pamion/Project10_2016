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

char subBuff[SUBBUFF_LEN][20];	//pole ukazatelù rozdìlených podle mezer
static char toASCII[3];
static short int i, j;					//pomocná promìnná pro indexaci
static short int paramsCount;
static short int saveComPort = UNKNOWN;
static short int recognized = TRUE;		//pomocná promìnná pro vypsání nápovìdy o neznámém pøíkazu
static short int badArguments = FALSE;	
static char ptemp[50];					//pomocná promìnná pro výpis
static short int valOut, channel;

void serialTask(void)
{
	
	int buff_stat = BUFFER_INIT;

	if ( bufferIsLineReady((struct T_buffer *) &buffIn) ) {
		i = 0;
		recognized = TRUE;
		badArguments = FALSE;
		
		//Vyètení øádku z bufferu
		buff_stat = BUFFER_SUCCESS;
		while (buff_stat == BUFFER_SUCCESS) {
			buff_stat = bufferReadWord((struct T_buffer *) &buffIn,  (char *)&subBuff[i++]);
		}
		paramsCount = i;
		for (i = paramsCount; i < SUBBUFF_LEN; i++) {
			subBuff[i++][0] = '\0';
		}
		
		//Vykonání základních pøíkazù
		if CHECK_COMMAND("help", 0) {
			
			if (paramsCount == 2) {
				if CHECK_COMMAND("help", 1) {
					showHelpHelp();
				} else if CHECK_COMMAND("comport", 1) {
					showComportHelp();
				} else if CHECK_COMMAND("meas", 1) {
					showMeasHelp();
				} else if CHECK_COMMAND("channels", 1) {
					showChannelsHelp();
				} else if CHECK_COMMAND("output", 1) {
					showOutputHelp();
				} else if CHECK_COMMAND("info", 1) {
					showInfoText();
				} else if CHECK_COMMAND("expconf", 1) {
					showExpConfHelp();
				} else if CHECK_COMMAND("defaults", 1) {
					showDefaultsHelp();
				} else {
					showHelpHelp();
				}

			} else { // if only name of command arrived
				showHelpHelp();
			}
			//End of HELP command

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
			memcpy(&publicConfigNew, &publicConfig, sizeof(publicConfig));
			memcpy(&publicConfig2Save, &publicConfig, sizeof(publicConfig));
			saveComPort = UNKNOWN;
		} else {
			recognized = FALSE;
		}
		
		// Standartní konfiguraèní pøíkazy
		if ( (statusMachine >= MACHINE_USER_CONFIGURATION) && (recognized == FALSE) ) {
			recognized = TRUE;
			
			if CHECK_COMMAND("comport", 0) {
				if (paramsCount != 1) {
					i = 1;
					if (saveComPort == FALSE) {
						saveComPort = UNKNOWN;
					}
					while ( (i < paramsCount) && (badArguments == FALSE) ) {
						if CHECK_COMMAND("-b", i) {
							if (validateInput(subBuff[i+1], VAL_INTEGER)) {
								publicConfigNew.comPortBaudrate = atoi(subBuff[i + 1]);
								if ( (publicConfigNew.comPortBaudrate >= 1200) & (publicConfigNew.comPortBaudrate <= 115200) ) {
									sprintf(ptemp, "->New baud rate will be %ld\r\n", publicConfigNew.comPortBaudrate);
									usart_write_line(USER_RS232, ptemp);
									i++;
								} else {
									publicConfigNew.comPortBaudrate = publicConfig2Save.comPortBaudrate;
									usart_write_line(USER_RS232, "Error: Baud rate must be between 1200 and 115200 baud.\r\n");
									badArguments = INVALID;
								}
							} else {
								publicConfigNew.comPortBaudrate = publicConfig2Save.comPortBaudrate;
								usart_write_line(USER_RS232, "Error: Baud rate must contain only numbers.\r\n");
								badArguments = INVALID;
							}
							 //end if "-b"
							 
						} else if CHECK_COMMAND("-h", i) {
							if CHECK_COMMAND("on", i + 1) {
								usart_write_line(USER_RS232, "->Hardware handshaking will be ON.\r\n");
								publicConfigNew.comPortHandshake = 1;
								i++;
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Hardware handshaking will be OFF.\r\n");
								publicConfigNew.comPortHandshake = 0;
								i++;
							} else {
								usart_write_line(USER_RS232, "Error: Hardware handshaking could be only on or off.\r\n");
								badArguments = INVALID;
							}
							//end if "-h"
						
						} else if CHECK_COMMAND("-s", i) {
							saveComPort = TRUE;
							 //end if "-s"
							 
						} else {
							i = paramsCount;
							badArguments = TRUE;
						}
						i++;
					} /* end-while trough params */
					
					if (badArguments == FALSE) {
						/* Confirm promt */
						if (saveComPort == UNKNOWN) {
							showComportWarning();
						}
						while (saveComPort == UNKNOWN) {
							if ( bufferIsLineReady((struct T_buffer *) &buffIn) ) {
								bufferReadWord((struct T_buffer *) &buffIn, (char *)&ptemp);
								if ( (ptemp[0] == 'Y') || (ptemp[0] == 'y') ) {
									saveComPort = TRUE;
									usart_write_line(USER_RS232, "Changes will be saved\r\n");
								} else if ( (ptemp[0] == 'N') || (ptemp[0] == 'n') ) {
									saveComPort = FALSE;
									usart_write_line(USER_RS232, "\r\n");
								}
							}
						}
					
						if (saveComPort == TRUE) {
							publicConfig2Save.comPortBaudrate = publicConfigNew.comPortBaudrate;
							publicConfig2Save.comPortHandshake = publicConfigNew.comPortHandshake;
							measTimeInfo(TRUE);
						}	
					} else {
						publicConfigNew.comPortBaudrate = publicConfig2Save.comPortBaudrate;
						publicConfigNew.comPortHandshake = publicConfig2Save.comPortHandshake;
					}
					
				} else {
					showComportHelp();
				}
				//End of COMPORT command
				
			} else if CHECK_COMMAND("meas", 0) {
				if (paramsCount != 1) {
					i = 1;
					while ( (i < paramsCount) && (badArguments == FALSE) ) {
						if CHECK_COMMAND("-t", i) {
							if (validateInput(subBuff[i+1], VAL_INTEGER)) {
								publicConfigNew.measNPLC = atoi(subBuff[i+1]);
								if ( (publicConfigNew.measNPLC >= 1) && (publicConfigNew.measNPLC <= 100) ) {
									sprintf(ptemp, "->New measurement time will be %d NPLC\r\n", publicConfigNew.measNPLC);
									usart_write_line(USER_RS232, ptemp);
									i++;
								} else { // not in range
									publicConfigNew.measNPLC = publicConfig2Save.measNPLC;
									usart_write_line(USER_RS232, "Error: Measurement time can be set between 1 and 100 NPLCs.\r\n");
									badArguments = INVALID;
								}
							} else { // not a number
								publicConfigNew.measNPLC = publicConfig2Save.measNPLC;
								usart_write_line(USER_RS232, "Error: Measurement time can be integer only.\r\n");
								badArguments = INVALID;
							}
							//end if "-t"
							
						} else if CHECK_COMMAND("-lf", i) {
							if CHECK_COMMAND("50", i+1) {
								usart_write_line(USER_RS232, "->Power line frequency will be set to 50 Hz.\r\n");
								publicConfigNew.measPowerLineFreq = 50;
								i++;
							} else if CHECK_COMMAND("60", i+1) {
								usart_write_line(USER_RS232, "->Power line frequency will be set to 60 Hz.\r\n");
								publicConfigNew.measPowerLineFreq = 60;
								i++;
							} else { //not 50 either 60 Hz
								usart_write_line(USER_RS232, "Error: Power line frequency could be only 50 or 60 Hz.\r\n");
								badArguments = INVALID;
							}
							//end if "-lf"
							
						} else if CHECK_COMMAND("-rn", i) {
							if CHECK_COMMAND("on", i+1) {
								usart_write_line(USER_RS232, "->Rounding will be enabled.\r\n");
								publicConfigNew.measRounding = 1;
								i++;
							} else if CHECK_COMMAND("off", i+1) {
								usart_write_line(USER_RS232, "->Rounding will be disabled.\r\n");
								publicConfigNew.measRounding = 0;
								i++;
							} else {
								usart_write_line(USER_RS232, "Error: Rounding could be only on or off.\r\n");
								badArguments = INVALID;
							}
							//end if "-rn"
							
						} else if CHECK_COMMAND("-sc", i) {
							if CHECK_COMMAND("on", i + 1) {
								usart_write_line(USER_RS232, "->Scientific notation will be enabled.\r\n");
								publicConfigNew.measScientific = 1;
								i++;
							} else if CHECK_COMMAND("off", i + 1) {
								usart_write_line(USER_RS232, "->Scientific notation will be disabled.\r\n");
								publicConfigNew.measScientific = 0;
								i++;
							} else {
								badArguments = INVALID;
							}
							//end if "-sc"
							
						} else {
							i = paramsCount;
							badArguments = TRUE;
						}
						i++;
						/* end-while trough params */
					}
					
					if (badArguments == FALSE) {
						//Revert all changes, when any problem with line occurs
						publicConfig2Save.measNPLC			= publicConfigNew.measNPLC;
						publicConfig2Save.measPowerLineFreq	= publicConfigNew.measPowerLineFreq;
						publicConfig2Save.measRounding		= publicConfigNew.measRounding;
						publicConfig2Save.measScientific	= publicConfigNew.measScientific;
						usart_write_line(USER_RS232, "The results will be sent out via RS-232 port in format:\r\n");
						outputStringExample( publicConfig2Save.outputPrefix, publicConfig2Save.outputSeparator, publicConfig2Save.outputSuffix,
												publicConfig2Save.outputLineEnding, publicConfig2Save.measRounding, publicConfig2Save.measScientific );
						usart_write_line(USER_RS232, "\r\n\r\n");
						measTimeInfo( FALSE );
					} else { //when valid arguments
						publicConfigNew.measNPLC			= publicConfig2Save.measNPLC;
						publicConfigNew.measPowerLineFreq	= publicConfig2Save.measPowerLineFreq;
						publicConfigNew.measRounding		= publicConfig2Save.measRounding;
						publicConfigNew.measScientific		= publicConfig2Save.measScientific;
						
					}
				} else { // if only name of command arrived
					showMeasHelp();
				}
				//End of MEAS command
				
			} else if CHECK_COMMAND("output", 0) {
				if (paramsCount != 1) {
					i = 1;
					while ( (i < paramsCount) && (badArguments == FALSE) ) {
						if CHECK_COMMAND("-p", i) {
							valOut = validateInput( subBuff[i+1], VAL_OUTPUT_STR );
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][++j] != '"') && (subBuff[i+1][j] != '\0') && (j < 9) )
									publicConfigNew.outputPrefix[j-1] = subBuff[i+1][j];
								publicConfigNew.outputPrefix[j-1] = '\0';
								usart_write_line(USER_RS232, "New prefix entered\r\n");
							} else {
								badArguments = INVALID;
							}
							i++;
							//end if "-p"
							
						} else if CHECK_COMMAND("-pa", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									publicConfigNew.outputPrefix[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								publicConfigNew.outputPrefix[j] = '\0';
								usart_write_line(USER_RS232, "New prefix entered\r\n");
							} else {
								badArguments = INVALID;
							}
							i++;
							//end if "-p"
							
						} else if CHECK_COMMAND("-s", i) {
							valOut = validateInput( subBuff[i+1], VAL_OUTPUT_STR );
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][++j] != '"') && (subBuff[i+1][j] != '\0') && (j < 9) )
									publicConfigNew.outputSeparator[j-1] = subBuff[i+1][j];
								publicConfigNew.outputSeparator[j-1] = '\0';
								usart_write_line(USER_RS232, "New separator entered\r\n");
							} else {
								badArguments = INVALID;
							}
							i++;
							//end if "-s"
							
						} else if CHECK_COMMAND("-sa", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									publicConfigNew.outputSeparator[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								publicConfigNew.outputSeparator[j] = '\0';
								usart_write_line(USER_RS232, "New separator entered\r\n");
							} else {
								badArguments = INVALID;
							}
							i++;
							//end if "-sa"
							
						} else if CHECK_COMMAND("-u", i) {
							valOut = validateInput( subBuff[i+1], VAL_OUTPUT_STR );
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][++j] != '"') && (subBuff[i+1][j] != '\0') && (j < 9) )
									publicConfigNew.outputSuffix[j-1] = subBuff[i+1][j];
								publicConfigNew.outputSuffix[j-1] = '\0';
								usart_write_line(USER_RS232, "New suffix entered\r\n");
							} else {
								badArguments = INVALID;
							}
							i++;
							//end if "-u"
							
						} else if CHECK_COMMAND("-ua", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									publicConfigNew.outputSuffix[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								publicConfigNew.outputSuffix[j] = '\0';
								usart_write_line(USER_RS232, "New suffix entered\r\n");
							} else {
								badArguments = INVALID;
							}
							i++;
							//end if "-ua"
							
						} else if CHECK_COMMAND("-l", i) {
							if CHECK_COMMAND("None", i + 1) {
								publicConfigNew.outputLineEnding[0] = '\0';
								usart_write_line(USER_RS232, "Line ending set to None.\r\n");
							} else if (CHECK_COMMAND("ETX", i + 1) || CHECK_COMMAND("etx", i + 1)) {
								publicConfigNew.outputLineEnding[0] = 0x03;
								publicConfigNew.outputLineEnding[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to ETX (End of Text).\r\n");
							} else if (CHECK_COMMAND("EOT", i + 1) || CHECK_COMMAND("eot", i + 1)) {
								publicConfigNew.outputLineEnding[0] = 0x04;
								publicConfigNew.outputLineEnding[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to EOT (End of Transmission).\r\n");
							} else if (CHECK_COMMAND("CR", i + 1) || CHECK_COMMAND("cr", i + 1)) {
								publicConfigNew.outputLineEnding[0] = 0x0d;
								publicConfigNew.outputLineEnding[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to CR (Carriage Return).\r\n");
							} else if (CHECK_COMMAND("LF", i + 1) || CHECK_COMMAND("lf", i + 1)){
								publicConfigNew.outputLineEnding[0] = 0x0a;
								publicConfigNew.outputLineEnding[1] = '\0';
								usart_write_line(USER_RS232, "Line ending set to LF (Line Feed).\r\n");
							} else if (CHECK_COMMAND("CRLF", i + 1) || CHECK_COMMAND("crlf", i + 1)) {
								publicConfigNew.outputLineEnding[0] = 0x0d;
								publicConfigNew.outputLineEnding[1] = 0x0a;
								publicConfigNew.outputLineEnding[2] = '\0';
								usart_write_line(USER_RS232, "Line ending set to CRLF (Carriage Return and Line Feed).\r\n");
							} else if (CHECK_COMMAND("LFCR", i + 1) || CHECK_COMMAND("lfcr", i + 1)) {
								publicConfigNew.outputLineEnding[0] = 0x0a;
								publicConfigNew.outputLineEnding[1] = 0x0d;
								publicConfigNew.outputLineEnding[2] = '\0';
								usart_write_line(USER_RS232, "Line ending set to LFCR (Line Feed and Carriage Return).\r\n");
							} else {
								usart_write_line(USER_RS232, "Error: Not valid line ending.\r\n");
								badArguments = INVALID;
							}
							i++;
							//end if "-l"
							
						} else if CHECK_COMMAND("-la", i) {
							valOut = validateInput(subBuff[i+1], VAL_HEX);
							if ( valOut == TRUE ) {
								j = 0;
								while ( (subBuff[i+1][2*j] != '\0') && (j < 18) ){
									toASCII[0] = subBuff[i+1][2*j];
									toASCII[1] = subBuff[i+1][2*j+1];
									toASCII[2] = '\n';
									publicConfigNew.outputLineEnding[j] = (char)strtoul(toASCII, NULL, 16);
									j++;
								}
								publicConfigNew.outputLineEnding[j] = '\0';
								usart_write_line(USER_RS232, "New line ending entered\r\n");
							} else {
								badArguments = INVALID;
							}
							i++;
							//end if "-ua"
							
						} else {
							i = paramsCount;
							badArguments = TRUE;
						}
						i++;
						/* end-while trough params */
					}
					
					if (badArguments == INVALID) {
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
					}
					if (badArguments == FALSE) {
						//Apply all changes, when NO problem with line occurs
						memcpy(&publicConfig2Save.outputPrefix,		&publicConfigNew.outputPrefix,		sizeof(publicConfigNew.outputPrefix));
						memcpy(&publicConfig2Save.outputSeparator,	&publicConfigNew.outputSeparator,	sizeof(publicConfigNew.outputSeparator));
						memcpy(&publicConfig2Save.outputSuffix,		&publicConfigNew.outputSuffix,		sizeof(publicConfigNew.outputSuffix));
						memcpy(&publicConfig2Save.outputLineEnding, &publicConfigNew.outputLineEnding,	sizeof(publicConfigNew.outputLineEnding));
							
						usart_write_line(USER_RS232, "The results will be sent out via RS-232 port in format:\r\n");
						outputStringExample( publicConfig2Save.outputPrefix, publicConfig2Save.outputSeparator, publicConfig2Save.outputSuffix,
						publicConfig2Save.outputLineEnding, publicConfig2Save.measRounding, publicConfig2Save.measScientific );
						usart_write_line(USER_RS232, "\r\n\r\n");
						measTimeInfo( FALSE );
					} else { 
						//Revert all changes, when ANY problem with line occurs
						memcpy(&publicConfigNew.outputPrefix,		&publicConfig2Save.outputPrefix,	 sizeof(publicConfigNew.outputPrefix));
						memcpy(&publicConfigNew.outputSeparator,	&publicConfig2Save.outputSeparator,  sizeof(publicConfigNew.outputSeparator));
						memcpy(&publicConfigNew.outputSuffix,		&publicConfig2Save.outputSuffix,	 sizeof(publicConfigNew.outputSuffix));
						memcpy(&publicConfigNew.outputLineEnding,	&publicConfig2Save.outputLineEnding, sizeof(publicConfigNew.outputLineEnding));				
					}
				} else { // if only name of command arrived
					showOutputHelp();
				}
				//End of OUTPUT command
				
			} else if CHECK_COMMAND("channels", 0) {
				if (paramsCount != 1) {
					i = 1;
					while ( (i < paramsCount) && (badArguments == FALSE) ) {
						if CHECK_COMMAND("-ae", i) { // ALL ENABLE
							publicConfigNew.channelsToogleMask = 0xFFFF;
							usart_write_line(USER_RS232, "All channels enabled\r\n");
							//end if "-ea"
							
						} else if CHECK_COMMAND("-ad", i) { // ALL DISABLE
							publicConfigNew.channelsToogleMask = 0x0000;
							usart_write_line(USER_RS232, "All channels disabled\r\n");
							//end if "-ed"
						
						} else if CHECK_COMMAND("-ce", i) { //CHANNEL ENABLE
							if (validateInput(subBuff[i+1], VAL_INTEGER)) {
								channel = atoi(subBuff[i+1]);
								if ( (channel >= 1) && (channel <= 16) ) {
									sprintf(ptemp, "->Channel %d will be enabled\r\n", channel);
									usart_write_line(USER_RS232, ptemp);
									publicConfigNew.channelsToogleMask = publicConfigNew.channelsToogleMask | (0x8000 >> (channel-1) );
									i++;
								} else { // not in range
									publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
									usart_write_line(USER_RS232, "Error: Channel number can be only between 1 and 16.\r\n");
									badArguments = INVALID;
								}
							} else { // not a number
								publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
								usart_write_line(USER_RS232, "Error: Channel number can be integer only.\r\n");
								badArguments = INVALID;
							}
							//end if "-ce"
						
						} else if CHECK_COMMAND("-cd", i) { //CHANNEL DISABLE
							if (validateInput(subBuff[i+1], VAL_INTEGER)) {
								channel = atoi(subBuff[i+1]);
								if ( (channel >= 1) && (channel <= 16) ) {
									sprintf(ptemp, "->Channel %d will be disabled\r\n", channel);
									usart_write_line(USER_RS232, ptemp);
									publicConfigNew.channelsToogleMask = publicConfigNew.channelsToogleMask & (~(0x8000 >> (channel-1) ));
									i++;
								} else { // not in range
									publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
									usart_write_line(USER_RS232, "Error: Channel number can be only between 1 and 16.\r\n");
									badArguments = INVALID;
								}
							} else { // not a number
								publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
								usart_write_line(USER_RS232, "Error: Channel number can be integer only.\r\n");
								badArguments = INVALID;
							}
							//end if "-cd"
						
						} else if CHECK_COMMAND("-ct", i) { //CHANNEL TOOGLE
							if (validateInput(subBuff[i+1], VAL_INTEGER)) {
								channel = atoi(subBuff[i+1]);
								if ( (channel >= 1) && (channel <= 16) ) {
									sprintf(ptemp, "->Channel %d will be toogled\r\n", channel);
									usart_write_line(USER_RS232, ptemp);
									publicConfigNew.channelsToogleMask = publicConfigNew.channelsToogleMask ^ (0x8000 >> (channel-1) );
									i++;
								} else { // not in range
									publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
									usart_write_line(USER_RS232, "Error: Channel number can be only between 1 and 16.\r\n");
									badArguments = INVALID;
								}
							} else { // not a number
								publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
								usart_write_line(USER_RS232, "Error: Channel number can be integer only.\r\n");
								badArguments = INVALID;
							}
							//end if "-ct"
						
						} else if CHECK_COMMAND("-cm", i) { //CHANNEL MASK
							valOut = validateInput(subBuff[i+1], VAL_BINARY);
							if (valOut == TRUE) {
								usart_write_line(USER_RS232, "->Channels enabled/disabled according to entered mask.\r\n");
								publicConfigNew.channelsToogleMask = strtol(subBuff[i+1], NULL, 2);
								i++;
							} else if (valOut == VAL_BIN_LEN_ERR) {
								publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
								usart_write_line(USER_RS232, "Error: Mask length need to be 16 characters long.\r\n");
								badArguments = INVALID;
							} else { // not a 0 / 1
								publicConfigNew.channelsToogleMask = publicConfig2Save.channelsToogleMask;
								usart_write_line(USER_RS232, "Error: Mask can only contain characters 0 and 1.\r\n");
								badArguments = INVALID;
							}
							//end if "-cm"
						
						} else {
							i = paramsCount;
							badArguments = TRUE;
						}
						i++;
						/* end-while trough params */
					}
					if (badArguments == FALSE) {
						//Apply all changes, when NO problem with line occurs
						publicConfig2Save.channelsToogleMask	= publicConfigNew.channelsToogleMask;
						usart_write_line(USER_RS232, "The results will be sent out via RS-232 port in format:\r\n");
						outputStringExample( publicConfig2Save.outputPrefix, publicConfig2Save.outputSeparator, publicConfig2Save.outputSuffix,
						publicConfig2Save.outputLineEnding, publicConfig2Save.measRounding, publicConfig2Save.measScientific );
						usart_write_line(USER_RS232, "\r\n\r\n");
						measTimeInfo( FALSE );
					} else { 
						//Revert all changes, when ANY problem with line occurs
						publicConfigNew.channelsToogleMask		= publicConfig2Save.channelsToogleMask;						
					}
				} else { // if only name of command arrived
					showChannelsHelp();
				}
				//End of CHANNELS command
				
			} else if CHECK_COMMAND("defaults", 0) {
				if (paramsCount == 2) {
					if CHECK_COMMAND("-s", 1) {
						resetPublicConfig(TRUE);
					} else {
						badArguments = TRUE;
					}
				} else if (paramsCount == 1) {
					showDefaultsWarning();
					i = UNKNOWN;
					while (i == UNKNOWN) {
						if ( bufferIsLineReady((struct T_buffer *) &buffIn) ) {
							bufferReadWord((struct T_buffer *) &buffIn, (char *)&ptemp);
							if ( (ptemp[0] == 'Y') || (ptemp[0] == 'y') ) {
								i = TRUE;
								resetPublicConfig(TRUE);
							} else if ( (ptemp[0] == 'N') || (ptemp[0] == 'n') ) {
								i = FALSE;
								usart_write_line(USER_RS232, "Reset canceled\r\n");
							}
						}
					}
				} else {
					badArguments = TRUE;
				}
				//End of DEFAULTS command
				
			} else if CHECK_COMMAND("expconf", 0) {
				exportConfiguration();
					
			} else if CHECK_COMMAND("discard", 0) {
				usart_write_line(USER_RS232, "Discarding changes...\r\n");
				usart_write_line(USER_RS232, "Restarting the luxmeter...\r\n\r\n");
				delay_ms(50); // Èekání, než se odešle celý øetìzec, na konci programu èekací smyèka nièemu nevadí...
				reset_do_soft_reset();
				// END of DISCARD command
			
			} else if ( CHECK_COMMAND("exit", 0) || CHECK_COMMAND("end", 0) || CHECK_COMMAND("save", 0) ) {
				usart_write_line(USER_RS232, "Saving changes...\r\n");
				
				/*TODO: SAVE */
				if (strncmp(publicConfig2Save.outputPrefix, publicConfig.outputPrefix, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputPrefix, &publicConfig2Save.outputPrefix, sizeof(publicConfig2Save.outputPrefix), TRUE);
					usart_write_line(USER_RS232, "->New prefix set.\r\n");
				}
				if (strncmp(publicConfig2Save.outputSeparator, publicConfig.outputSeparator, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputSeparator, &publicConfig2Save.outputSeparator, sizeof(publicConfig2Save.outputSeparator), TRUE);
					usart_write_line(USER_RS232, "->New separator set.\r\n");
				}
				if (strncmp(publicConfig2Save.outputSuffix, publicConfig.outputSuffix, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputSuffix, &publicConfig2Save.outputSuffix, sizeof(publicConfig2Save.outputSuffix), TRUE);
					usart_write_line(USER_RS232, "->New suffix set.\r\n");
				}
				if (strncmp(publicConfig2Save.outputLineEnding, publicConfig.outputLineEnding, 8) != 0 ) {
					flashc_memcpy(&publicConfig.outputLineEnding, &publicConfig2Save.outputLineEnding, sizeof(publicConfig2Save.outputLineEnding), TRUE);
					usart_write_line(USER_RS232, "->New line ending set.\r\n");
				}
				
				if (publicConfig2Save.measNPLC != publicConfig.measNPLC) {
					flashc_memcpy(&publicConfig.measNPLC, &publicConfig2Save.measNPLC, sizeof(publicConfig2Save.measNPLC), TRUE);
					sprintf(ptemp, "->Measurement time set to %2d\r\n", publicConfig2Save.measNPLC);
					usart_write_line(USER_RS232, ptemp);
				}
				
				if (publicConfig2Save.measPowerLineFreq != publicConfig.measPowerLineFreq) {
					flashc_memcpy(&publicConfig.measPowerLineFreq, &publicConfig2Save.measPowerLineFreq, sizeof(publicConfig2Save.measPowerLineFreq), TRUE);
					sprintf(ptemp, "->Power line frequency set to %2d\r\n", publicConfig2Save.measPowerLineFreq);
					usart_write_line(USER_RS232, ptemp);
				}
				
				if (publicConfig2Save.measRounding != publicConfig.measRounding) {
					flashc_memcpy(&publicConfig.measRounding, &publicConfig2Save.measRounding, sizeof(publicConfig2Save.measRounding), TRUE);
					usart_write_line(USER_RS232, "->Rounding set to ");
					if (publicConfig2Save.measRounding == 1) {
						usart_write_line(USER_RS232, "ON\r\n");
					} else {
						usart_write_line(USER_RS232, "OFF\r\n");
					}
				}
				
				if (publicConfig2Save.measScientific != publicConfig.measScientific) {
					flashc_memcpy(&publicConfig.measScientific, &publicConfig2Save.measScientific, sizeof(publicConfig2Save.measScientific), TRUE);
					usart_write_line(USER_RS232, "->Scientific notation set to ");
					if (publicConfig2Save.measScientific == 1) {
						usart_write_line(USER_RS232, "ON\r\n");
					} else {
						usart_write_line(USER_RS232, "OFF\r\n");
					}
				}
				
				if (publicConfig2Save.channelsToogleMask != publicConfig.channelsToogleMask) {
					enabledChannels = channelCount(publicConfig2Save.channelsToogleMask);
					flashc_memcpy(&publicConfig.channelsToogleMask, &publicConfig2Save.channelsToogleMask, sizeof(publicConfig2Save.channelsToogleMask), TRUE);
					usart_write_line(USER_RS232, "->Enabling/disabling channels.\r\n");
				}				
				
				if (saveComPort == TRUE) {
					
					if (publicConfig2Save.comPortBaudrate != publicConfig.comPortBaudrate) {
						flashc_memcpy(&publicConfig.comPortBaudrate, &publicConfig2Save.comPortBaudrate, sizeof(publicConfig2Save.comPortBaudrate), TRUE);
						sprintf(ptemp, "->Baud rate set to %2ld\r\n", publicConfig2Save.comPortBaudrate);
						usart_write_line(USER_RS232, ptemp);
					}
					
					if (publicConfig2Save.comPortHandshake != publicConfig.comPortHandshake) {
						flashc_memcpy(&publicConfig.comPortHandshake, &publicConfig2Save.comPortHandshake, sizeof(publicConfig2Save.comPortHandshake), TRUE);
						usart_write_line(USER_RS232, "->RTS/CTS set to ");
						if (publicConfig2Save.comPortHandshake == 1) {
							usart_write_line(USER_RS232, "ON\r\n");
						} else {
							usart_write_line(USER_RS232, "OFF\r\n");
						}
					}
				} // if (saveComPort == TRUE)				
				
				usart_write_line(USER_RS232, "Restarting the luxmeter...\r\n\r\n");
				delay_ms(50); // Èekání, než se odešle celý øetìzec, na konci programu èekací smyèka nièemu nevadí...
				reset_do_soft_reset();
				// END of END/EXIT commands
				
			} else {
				recognized = FALSE;
			}
		} /* END of standart config. commands */
		

		/* Skrytá tovární nastavení */
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
		if ( (recognized == FALSE) && (statusMachine >= MACHINE_USER_CONFIGURATION) && (&subBuff[0][0] != NULL) ) {
			usart_write_line(USER_RS232, "Error: Bad command\r\n");
			usart_write_line(USER_RS232, "Enter \"help\" to get list of available commands.\r\n");
#ifdef DEBUG
			usart_write_line(USER_RS232, "\r\nDebug info: \r\n");
			for ( i=0; i < paramsCount; i++ ) {
				usart_write_line(USER_RS232, subBuff[i]);
				usart_write_line(USER_RS232, " ");
			}
#endif //DEBUG
		} else
		
		// Argument nerozpoznán - bìhem konfigurace - nepøišel znak enter
		if ( (badArguments == TRUE) && (statusMachine >= MACHINE_USER_CONFIGURATION) && (&subBuff[0][0] != NULL) ) {
			usart_write_line(USER_RS232, "Error: Bad argument.\r\n");
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
}