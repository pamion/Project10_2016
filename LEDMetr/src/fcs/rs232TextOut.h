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



#endif /* RS232TEXTOUT_H_ */