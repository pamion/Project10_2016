/*
 * \file serialTask.h
 * \brief Separátní soubor pro funkci SerialTask
 *
 * \author bostik
 *
 */


#ifndef SERIALTASK_H_
#define SERIALTASK_H_

//Promìnné pro naèítání øetìzce
extern volatile struct T_buffer buffIn;
extern volatile char buffInChar[RS232_BUFF_IN_SIZE];
/*
extern volatile short int statusRS232;
extern volatile short int pozRS232;
extern volatile char bufferRS232[100];
*/
extern volatile short int statusMachine;
extern nvram_data_t1 hiddenConfig;
extern nvram_data_t2 publicConfig;

/*
 * \brief Obsluha sériové linky
 *
 */
void serialTask(void);



#endif /* SERIALTASK_H_ */