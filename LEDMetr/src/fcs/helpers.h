/*
 * helpers.h
 *
 * Created: 09.10.2016 21:34:27
 *  Author: bostik
 */ 


#ifndef HELPERS_H_
#define HELPERS_H_

extern nvram_data_t1 hiddenConfig;
extern nvram_data_t2 publicConfig;

extern char pref[9];
extern char sepa[9];
extern char suff[9];
extern char lend[9];
extern volatile int print_sec;
extern volatile uint16_t AD_Data_Values[16];
extern double Brightness;

/*
 * \brief Zvedne èíslo kanálu o 1 nahoru
 * 
 * \param Ukazatel na èíslo kanálu (globální)
 */
void setChannelUp (uint8_t *CurrentChannel);

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
 * \brief Zobrazí výstup mìøení na RS-232
 * 
 */
void measTask(void);

#endif /* HELPERS_H_ */