/*
 * setup.h
 *
 * Created: 09.10.2016 21:57:56
 *  Author: bosti
 */ 


#ifndef SETUP_H_
#define SETUP_H_

extern volatile avr32_tc_t *tc;

/*
 * \brief Konfiguruje GPIO pro adresov� sb�rnice multiplexeru a povol� je
 * 
 */
void enable_Multiplexer(void);

/*
 * \brief Konfiguruje a inicializuje SPI0 pro AD p�evodn�k
 * 
 */
void enable_AD_spi(void);

/*
 * \brief P�epne linku pro v�b�r kan�lu multiplexeru na ��danou hodnotu
 *
 */
void SwitchMultiplexerToChannel(uint8_t *Channel);

/*
 * \brief P�iprav� cel� MCU pro b�h
 *
 */
void mainInit(void);

#endif /* SETUP_H_ */