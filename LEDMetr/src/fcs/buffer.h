/*
 * buffer.h
 *
 * Created: 30.11.2016 18:43:50
 *  Author: bosti
 */ 


#ifndef BUFFER_H_
#define BUFFER_H_

#define BUFFER_INIT				-1
#define BUFFER_NO_LINES			0
#define BUFFER_ERROR			1
#define BUFFER_SUCCESS			2
#define BUFFER_LINE_END			3

struct T_buffer {
	int pozRead;
	int pozWrite;
	int size;
	int usedSpace;
	int linesCount;
	char *data;
};

void bufferInit ( struct T_buffer *buff, char *dataIn, int sizeIn);
int bufferNextPosition (struct T_buffer *buff, int position);
int bufferPrevPosition (struct T_buffer *buff, int position);
int bufferWriteChar (struct T_buffer *buff, char *c);
int bufferReadChar (struct T_buffer *buff, char *c);
int bufferReadWord (struct T_buffer *buff, char *c);
bool bufferIsEmpty (struct T_buffer *buff);
bool bufferIsFull (struct T_buffer *buff);
bool bufferIsLineReady (struct T_buffer *buff);

#endif /* BUFFER_H_ */