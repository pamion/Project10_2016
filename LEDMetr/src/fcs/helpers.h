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
extern volatile int print_sec;
extern volatile uint16_t AD_Data_Values2Send[16];
extern double Brightness;

int channelCount(uint16_t channel);

/*
 * \brief Zvedne èíslo kanálu o 1 nahoru
 * 
 * \param Ukazatel na èíslo kanálu (globální)
 */
int setChannelUp (uint8_t *CurrentChannel);

/*
 * \brief Sníží èíslo kanálu o 1 dolù
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
 * \brief Upraví výstupní char tak, aby obsahoval pouze platná desetinná místa
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
 * \brief Zobrazí výstup mìøení na RS-232
 * 
 * \return 0 when succes, return 1 when not succes
 *
 */
short int measTask(void);

#endif /* HELPERS_H_ */