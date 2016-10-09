/*
 * helpers.h
 *
 * Created: 09.10.2016 21:34:27
 *  Author: bostik
 */ 


#ifndef HELPERS_H_
#define HELPERS_H_

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

#endif /* HELPERS_H_ */