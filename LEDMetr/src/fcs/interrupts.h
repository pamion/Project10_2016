 /*
 * \file interrupts.h
 * \brief Obsluha pøerušení
 *
 * \author bostik
 *
 */



#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

extern volatile U32 tc_tick;
extern volatile int print_sec;
extern volatile int ChannelSwitchedFlag;
extern volatile uint16_t AD_Data;
extern volatile uint32_t ADReadsSummator;
extern volatile int AveragedReadsCounter;
extern volatile uint16_t AD_Data_Values[16];
extern volatile uint8_t MultiplexerChannel;
extern volatile int ChannelSwitchingCounter;
extern volatile short int statusRS232;
extern volatile short int pozRS232;
extern volatile short int afterFirstQuote;
extern volatile char bufferRS232[100];

/*
 * \brief Pøerušení pro zpracování A/D pøevodu
 *
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
#pragma handler = ADRead_TC_IRQ_GROUP, 1
__interrupt
#endif
void ADRead_irq(void);

/*
 * \brief Pøerušení pøi pøíchodu zprávy po RS-232
 *
 */
#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined(__ICCAVR32__)
__interrupt
#endif
void usart_int_handler(void);



#endif /* INTERRUPTS_H_ */