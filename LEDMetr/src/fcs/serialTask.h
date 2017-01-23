/*
 * \file serialTask.h
 * \brief Separ�tn� soubor pro funkci SerialTask
 *
 * \author bostik
 *
 */


#ifndef SERIALTASK_H_
#define SERIALTASK_H_

//Prom�nn� pro na��t�n� �et�zce
extern volatile struct T_buffer buffIn;
extern volatile char buffInChar[RS232_BUFF_IN_SIZE];
extern volatile short int statusMachine;
extern short int enabledChannels;
extern nvram_data_t1 hiddenConfig;
extern nvram_data_t1 hiddenConfigNew;
extern nvram_data_t1 hiddenConfig2Save;
extern nvram_data_t2 publicConfig;
extern nvram_data_t2 publicConfigNew;
extern nvram_data_t2 publicConfig2Save;

/*
 * \brief Obsluha s�riov� linky
 *
 */
void serialTask(void);



#endif /* SERIALTASK_H_ */