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
extern volatile int ChannelSwitchedFlag;
extern volatile int ChannelSwitchingCounter;
extern volatile int dataReady2send;
extern volatile uint16_t AD_Data;
extern volatile uint16_t AD_Data_Values[16];
extern volatile uint32_t ADReadsSummator;
extern volatile int AveragedReadsCounter;
extern volatile uint8_t MultiplexerChannel;
extern nvram_data_t1 hiddenConfig;
extern nvram_data_t2 publicConfig;

extern volatile char buffInChar[RS232_BUFF_IN_SIZE];
extern volatile struct T_buffer buffIn;

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