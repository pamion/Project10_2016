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
extern nvram_data_t2 publicConfig;

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



#endif /* RS232TEXTOUT_H_ */