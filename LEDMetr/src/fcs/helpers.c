 /*
 * \file helpers.c
 * \brief Pomocné funkce
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

uint16_t mask;

int channelCount(uint16_t channel) {
	short count, i;
	count = 0;
	for (i=0; i<16; i++) {
		count = count + ((channel >> i) & 0x0001);
	}
	return count;
}


int setChannelUp (uint8_t *CurrentChannel)
{
	int ret = FALSE;
	do {
		(*CurrentChannel)++;
		*CurrentChannel=(*CurrentChannel)%16;
		
		mask = publicConfig.channelsToogleMask;
		mask = (mask << (*CurrentChannel)) & 0x8000;
		
		if ( (*CurrentChannel)==15 ) {
			ret = TRUE;
		}
		
	} while ( mask != 0x8000 );
	return ret;
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

void takeCareOfValidDecimalPaces(char *outputString) {
		short int i=0;
		short int N = 1;
		short int decimal = FALSE;
		
		while ( (outputString[i] != '\0') && (outputString[i] != 'E') ) {
			
			if ( (outputString[i] >= '0') && (outputString[i] <= '9') ) {
				if ( N > N_VALID_DEC_PLACES ) {
					if ( decimal == TRUE) {
						outputString[i] = '\0';
					} else {
						outputString[i] = '0';
					}	
				}
				N++;
			} else if (outputString[i] == '.') {
				decimal = TRUE;
				if ( N > N_VALID_DEC_PLACES ) {
					outputString[i] = '\0';
				}
			}
			i++;
		}
		//if scientific notation occurs, get rid of leading zero
		if ( (outputString[i++] == 'E') && (outputString[++i] == '0') ) {
			while (outputString[++i] != '\0') {
				outputString[i-1] = outputString[i];
			}
			outputString[i-1] = '\0';
		}
			
}

int validateInput(char *str, short type) {
	int i;
	if (type == VAL_INTEGER) {
		i = 0;
		while( str[i] != NULL ) {
			if ( (str[i] < '0') | (str[i] > '9') ) {
				return FALSE;
			}
			i++;
		}
		return TRUE;
	} else if (type == VAL_BINARY) {
		if (strlen(str) != 16) {
			return VAL_BIN_LEN_ERR;
		} else {
			i = 0;
			while( str[i] != NULL ) {
				if ( (str[i] != '0') && ( str[i] != '1' ) ) {
					return FALSE;
				}
				i++;
			}
			return TRUE;
		}
	} else if ( type == VAL_HEX ) {
		if ( ( (strlen(str) % 2) == 1 ) || ( strlen(str) == 0 ) ){
			return VAL_ODD_CHARS;
		} else if ( strlen(str) > 16 ) {
			return VAL_HEX_LEN_ERR;
		} else {
			i = 0;
			while( str[i] != NULL ) {
				if ( !( 
					( (str[i] >= '0') & (str[i] <= '9') ) |
					( (str[i] >= 'a') & (str[i] <= 'f') ) |
					( (str[i] >= 'A') & (str[i] <= 'F') )
				   ) ) {
					   return VAL_BAD_HEX;
				}
				i++;
			}
			return TRUE;
		}
	} else if ( type == VAL_OUTPUT_STR ) {
		if ( strlen(str) > 10 ) {
			return VAL_STR_LEN_ERR;
		} else if ( ( str[0] != '"' ) || ( str[ strlen(str)-1 ] != '"' ) ) {
			return VAL_STR_QOTATION_ERR;
		} else {
			return TRUE;
		}
	} else {
		return FALSE;
	}
}

short int measTask(void) {
	int j, timeout;
	char ptemp[20];
	
	usart_write_line(USER_RS232, publicConfig.outputPrefix);				//Start of line
			
	for (j=0; j<16; j++) {
		ADToBrightness(&Brightness, AD_Data_Values2Send[j]);
		
		timeout = USART_DEFAULT_TIMEOUT;
		do
		{
			if (!timeout--) return 1;
		} while ( !usart_tx_ready(USER_RS232) );
		
		if (Brightness < 0.25){
			Brightness = 0;
		}
		
		if ( publicConfig.measScientific == 1 ) {
			sprintf(ptemp, "%1.2E", Brightness);
		} else if ( publicConfig.measRounding == 1 ) {
			sprintf(ptemp, "%1.0f", Brightness);
		} else{
			sprintf(ptemp, "%1.2f", Brightness);
		}
		
		//Take care of valid numbers
		takeCareOfValidDecimalPaces( ptemp );
				
		usart_write_line(USER_RS232, ptemp);								//Print one value
		if (j<15)
			usart_write_line(USER_RS232, publicConfig.outputSeparator);	//Print separator
		else
			usart_write_line(USER_RS232, publicConfig.outputSuffix);		//Print suffix
	}
	usart_write_line(USER_RS232, publicConfig.outputLineEnding);			//Print End of line
	return 0;
}