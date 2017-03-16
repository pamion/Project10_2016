 /*
 * \file helpers.h
 * \brief Pomocné funkce
 *
 * \author bostik
 *
 */


#ifndef HELPERS_H_
#define HELPERS_H_

extern nvram_data_t1 hiddenConfig;
extern nvram_data_t2 publicConfig;
extern nvram_data_t2 publicConfigNew;
extern nvram_data_t1 hiddenConfigNew;
extern volatile int print_sec;
extern volatile uint16_t AD_Data_Values2Send[16];
extern double Brightness;

/*
 * \brief Varianta funkce usart_write_line limitovaná na 8 znakù
 * 
 * \param usart   Base address of the USART instance.
 * \param string  String to be written.
 */
void usart_write_line_8chars(volatile avr32_usart_t *usart, const char *string);

/*
 * \brief Vıpoèet èasu mìøení
 * 
 * \return (float) délka mìøení
 */
int getMeasTime ( int *cycle );

/*
 * \brief Spoète poèet povolenıch kanálù
 * 
 * \param uint16 _t maska kanálù
 */
int channelCount(uint16_t channel);

/*
 * \brief Resetuje veøejnou pamì a pøípadnì restartuje luxmetr
 * 
 * \param BOOL zda po resetu pamìt ihned provést restart
 */
void resetPublicConfig(short restart);

/*
 * \brief Zvedne èíslo kanálu o 1 nahoru
 * 
 * \param Ukazatel na èíslo kanálu (globální)
 * \return TRUE kdykoliv jsou namìøeny všechny povolené kanály
 */
int setChannelUp (uint8_t *CurrentChannel);

/*
 * \brief Sníí èíslo kanálu o 1 dolù
 * 
 * \param Ukazatel na èíslo kanálu (globální)
 */
void setChannelDown (uint8_t *CurrentChannel);

/*
 * \brief Pøevede hodnotu A/D pøevodníku na Svítivost v lx
 * 
 * \param Svítivost [ lx ]
 * \param Hodnota AD pøevodníku
 */
void ADToBrightness(double *pBrightness, int AD_Data);

/*
 * \brief Upraví vıstupní char tak, aby obsahoval pouze platná desetinná místa
 *
 * \param Pole znakù pro úpravu
 * 
 */
void takeCareOfValidDecimalPaces(char *outputString);

/*
 * \brief Validuje vstupní øetìzec
 *
 */
int validateInput(char *str, short type) ;

/*
 * \brief Zobrazí vıstup mìøení na RS-232
 * 
 * \return 0 when succes, return 1 when not succes
 *
 */
short int measTask(void);

#endif /* HELPERS_H_ */