/*
 * \file serialTask.h
 * \brief Separ�tn� soubor pro funkci SerialTask
 *
 * \author bostik
 *
 */


#ifndef SERIALTASK_H_
#define SERIALTASK_H_

extern volatile short int statusRS232;
extern volatile short int pozRS232;
extern volatile char bufferRS232[100];
extern volatile short int statusMachine;
extern nvram_data_t1 hiddenConfig;
extern nvram_data_t2 publicConfig;

/*
 * \brief Obsluha s�riov� linky
 *
 */
void serialTask(void);



#endif /* SERIALTASK_H_ */