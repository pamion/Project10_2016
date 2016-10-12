/*
 * helpers.h
 *
 * Created: 09.10.2016 21:34:27
 *  Author: bostik
 */ 


#ifndef HELPERS_H_
#define HELPERS_H_

extern volatile short int statusRS232;
extern volatile char bufferRS232[100];
extern volatile short int statusMachine;
extern nvram_data_t1 hiddenConfig;
extern nvram_data_t2 publicConfig;

/*
 * \brief Zvedne ��slo kan�lu o 1 nahoru
 * 
 * \param Ukazatel na ��slo kan�lu (glob�ln�)
 */
void setChannelUp (uint8_t *CurrentChannel);

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
 * \brief Obsluha s�riov� linky
 *
 */
void serialTask(void);

#endif /* HELPERS_H_ */