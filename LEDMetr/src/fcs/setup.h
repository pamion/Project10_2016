 /*
 * \file setup.h
 * \brief Funkce spojené s nábìhem MCU
 *
 * \author bostik
 *
 */


#ifndef SETUP_H_
#define SETUP_H_

extern volatile avr32_tc_t *tc;
extern char pref[9];
extern char sepa[9];
extern char suff[9];
extern char lend[9];

/*
 * \brief Konfiguruje GPIO pro adresové sbìrnice multiplexeru a povolí je
 * 
 */
void enable_Multiplexer(void);

/*
 * \brief Konfiguruje a inicializuje SPI0 pro AD pøevodník
 * 
 */
void enable_AD_spi(void);

/*
 * \brief Pøepne linku pro výbìr kanálu multiplexeru na žádanou hodnotu
 *
 */
void SwitchMultiplexerToChannel(uint8_t *Channel);

/*
 * \brief Pøipraví celý MCU pro bìh
 *
 */
void mainInit(void);

#endif /* SETUP_H_ */