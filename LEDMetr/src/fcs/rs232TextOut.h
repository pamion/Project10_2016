/*
 * \file rs232TextOut.h
 * \brief Všechny funkce pro vypsání textù v konfiguraèním módu
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
 * /brief Funkce pro zobrazení textù vyvolaných pøi zavolání pøíkazu EXPCONF
 *
 */
void showExpConfHelp(void);

/*
 * /brief Funkce pro zobrazení textù vyvolaných pøi zavolání pøíkazu HELP
 *
 */
void showHelpHelp(void);

/*
 * /brief Funkce pro zobrazení textù vyvolaných pøi zavolání pøíkazu INFO
 *
 */
void showInfoText(void);

/*
 * /brief Funkce pro zobrazení textù vyvolaných pøi zavolání pøíkazu CONFIG
 *
 */
void showConfigText(void);

/*
 * /brief Funkce pro zobrazení nápovìdy pøíkazu DEFAULTS
 *
 */
void showDefaultsHelp(void);

/*
 * /brief Funkce pro zobrazení warování na zmìnu pøi použití pøíkazu DEFAULTS
 *
 */
void showDefaultsWarning(void);

/*
 * /brief Funkce pro zobrazení nápovìdy pøíkazu COMPORT
 *
 */
void showComportHelp(void);

/*
 * /brief Funkce pro zobrazení warování na zmìnu pøi použití pøíkazu COMPORT
 *
 */
void showComportWarning(void);

/*
 * /brief Funkce pro zobrazení nápovìdy pøíkazu MEAS
 *
 */
void showMeasHelp(void);

/*
 * /brief Funkce pro zobrazení nápovìdy pøíkazu OUTPUT
 *
 */
void showOutputHelp(void);

/*
 * /brief Funkce pro zobrazení nápovìdy pøíkazu CHANNELS
 *
 */
void showChannelsHelp(void);

/*
 * /brief Funkce pro export konfigurace do dávky
 *
 */
void exportConfiguration(void);

/*
 * /brief Funkce pro zobrazení stringù obsahující netisknutelné znaky
 *
 */
void  hexToStringRepresentation(char *string );

/*
 * /brief Ukázka výstupního øetìzce
 *
 */
void outputStringExample( char *pre, char *se, char *su, char *le, short int rn, short int sc );

/*
 * /brief Vypsání odhadu èasu mìøení
 *
 */
void measTimeInfo( void );

#endif /* RS232TEXTOUT_H_ */