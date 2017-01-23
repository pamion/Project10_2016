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
extern nvram_data_t1 hiddenConfig2Save;
extern nvram_data_t2 publicConfig;
extern nvram_data_t2 publicConfig2Save;

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
 * /brief Funkce pro zobrazen� n�pov�dy p��kazu DEFAULTS
 *
 */
void showDefaultsHelp(void);

/*
 * /brief Funkce pro zobrazen� warov�n� na zm�nu p�i pou�it� p��kazu DEFAULTS
 *
 */
void showDefaultsWarning(void);

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
 * /brief Funkce pro zobrazen� n�pov�dy p��kazu CHANNELS
 *
 */
void showChannelsHelp(void);

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
void measTimeInfo( void );

#endif /* RS232TEXTOUT_H_ */