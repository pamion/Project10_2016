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
	} 
	else if ( type == VAL_HEX ) {
		if ( ( (strlen(str) % 2) == 1 ) || ( strlen(str) == 0 ) ){
			return FALSE;
		} else {
			while( str[i] != NULL ) {
				if ( !( 
					( (str[i] >= '0') & (str[i] <= '9') ) |
					( (str[i] >= 'a') & (str[i] <= 'f') ) |
					( (str[i] >= 'A') & (str[i] <= 'F') )
				   ) ) {
					   return FALSE;
				}
				i++;
			}
			return TRUE;
		}
	} else {
		return TRUE;
	}
}

short int measTask(void) {
	int j, timeout;
	char ptemp[20];
	
	usart_write_line(USER_RS232, pref);					//Start of line
			
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
			sprintf(ptemp, "%1.4f", Brightness);
		}
		
		//Take care of valid numbers
		takeCareOfValidDecimalPaces( ptemp );
				
		usart_write_line(USER_RS232, ptemp);			//Print one value
		if (j<15)
			usart_write_line(USER_RS232, sepa);			//Print separator
		else
			usart_write_line(USER_RS232, suff);			//Print suffix
	}
	usart_write_line(USER_RS232, lend);					//Print End of line
	return 0;
}