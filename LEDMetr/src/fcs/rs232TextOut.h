/*
 * \file rs232TextOut.h
 * \brief V�echny funkce pro vyps�n� text� v konfigura�n�m m�du
 *
 * \author bostik
 *
 */


#ifndef RS232TEXTOUT_H_
#define RS232TEXTOUT_H_

extern nvram_data_t1 hiddenConfig;
extern nvram_data_t2 publicConfig;
/*
extern char pref[9];
extern char sepa[9];
extern char suff[9];
extern char lend[9];
*/

/*
 * /brief Funkce pro zobrazen� text� vyvolan�ch p�i zavol�n� p��kazu INFO
 *
 */
void showInfoText(void);

/*
 * /brief Funkce pro zobrazen� text� vyvolan�ch p�i zavol�n� p��kazu CONFIG
 *
 */
void showConfigText(void);

/*
 * /brief Funkce pro zobrazen� n�pov�dy p��kazu COMPORT
 *
 */
void showComportHelp(void);

/*
 * /brief Funkce pro zobrazen� warov�n� na zm�nu p�i pou�it� p��kazu COMPORT
 *
 */
void showComportWarning(void);

/*
 * /brief Funkce pro zobrazen� n�pov�dy p��kazu MEAS
 *
 */
void showMeasHelp(void);

/*
 * /brief Funkce pro zobrazen� n�pov�dy p��kazu OUTPUT
 *
 */
void showOutputHelp(void);

/*
 * /brief Funkce pro zobrazen� string� obsahuj�c� netisknuteln� znaky
 *
 */
void  hexToStringRepresentation(char *string );

/*
 * /brief Uk�zka v�stupn�ho �et�zce
 *
 */
void outputStringExample( char *pre, char *se, char *su, char *le, short int rn, short int sc );

/*
 * /brief Vyps�n� odhadu �asu m��en�
 *
 */
void measTimeInfo( short int NPLC, short int PLFreq, uint16_t settlingTime, uint16_t toogleMask, uint16_t baudRate );

#endif /* RS232TEXTOUT_H_ */