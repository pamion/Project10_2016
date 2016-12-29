 /*
 * \file helpers.h
 * \brief Pomocn� funkce
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

/*
 * \brief Spo�te po�et povolen�ch kan�l�
 * 
 * \param uint16 _t maska kan�l�
 */
int channelCount(uint16_t channel);

/*
 * \brief Resetuje ve�ejnou pam� a p��padn� restartuje luxmetr
 * 
 * \param BOOL zda po resetu pam�t ihned prov�st restart
 */
void resetPublicConfig(short restart);

/*
 * \brief Zvedne ��slo kan�lu o 1 nahoru
 * 
 * \param Ukazatel na ��slo kan�lu (glob�ln�)
 * \return TRUE kdykoliv jsou nam��eny v�echny povolen� kan�ly
 */
int setChannelUp (uint8_t *CurrentChannel);

/*
 * \brief Sn�� ��slo kan�lu o 1 dol�
 * 
 * \param Ukazatel na ��slo kan�lu (glob�ln�)
 */
void setChannelDown (uint8_t *CurrentChannel);

/*
 * \brief P�evede hodnotu A/D p�evodn�ku na Sv�tivost v lx
 * 
 * \param Sv�tivost [ lx ]
 * \param Hodnota AD p�evodn�ku
 */
void ADToBrightness(double *pBrightness, int AD_Data);

/*
 * \brief Uprav� v�stupn� char tak, aby obsahoval pouze platn� desetinn� m�sta
 *
 * \param Pole znak� pro �pravu
 * 
 */
void takeCareOfValidDecimalPaces(char *outputString);

/*
 * \brief Validuje vstupn� �et�zec
 *
 */
int validateInput(char *str, short type) ;

/*
 * \brief Zobraz� v�stup m��en� na RS-232
 * 
 * \return 0 when succes, return 1 when not succes
 *
 */
short int measTask(void);

#endif /* HELPERS_H_ */