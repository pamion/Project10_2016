/*
 * buffer.c
 *
 * Created: 30.11.2016 18:43:37
 *  Author: bosti
 */ 

#include <asf.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "buffer.h"

void bufferInit ( struct T_buffer *buff, char *dataIn, int sizeIn) {
	buff->pozRead = 0;
	buff->pozWrite = 0;
	buff->usedSpace = 0;
	buff->linesCount = 0;
	buff->size = sizeIn;
	buff->data = dataIn;
}

int bufferNextPosition (struct T_buffer *buff, int position) {
	return (position + 1) % buff->size;
}

int bufferPrevPosition (struct T_buffer *buff, int position) {
	return (buff->size + position - 1) % buff->size;
}

int bufferWriteChar (struct T_buffer *buff, char *c) {
	int prevPoz;
	if ( bufferIsFull(buff) )	{
		/* if overflow buffer we need to terminate buffer and try to load the data, even wrong */
		if ( (c=='\r') || (c=='\n') ) {
			buff->pozWrite = bufferPrevPosition(buff, buff->pozWrite);
			buff->usedSpace--;
			/* CRLF or LFCR handling only as one line ending */
			if ( ( (buff->data[buff->pozWrite]=='\r') || (buff->data[buff->pozWrite]=='\n') ) && (buff->data[buff->pozWrite]!=c) ) {
				buff->linesCount--;
			}
		} else {
			return BUFFER_ERROR;
		}
	}
	
	/* save character to the buffer */
	buff->data[buff->pozWrite] = c;
	
	/* End of the line */
	prevPoz = bufferPrevPosition(buff, buff->pozWrite);
	if ( (c=='\r') || (c=='\n') ) {
		/* CRLF or LFCR handling only as one line ending */
		
		if ( ( (buff->data[prevPoz]=='\r') || (buff->data[prevPoz]=='\n') ) && (buff->data[prevPoz]!=c) ) {
			buff->usedSpace--;
			buff->linesCount--;
			buff->pozWrite = prevPoz;
		}
		buff->linesCount++;
	}
	
	/* deals with space-space or space-newline situations */
	if ( ( (buff->data[buff->pozWrite] == '\r') || (buff->data[buff->pozWrite] == '\n') || (buff->data[buff->pozWrite] == ' ') ) && (buff->data[prevPoz] == ' ') ) {
		buff->data[prevPoz] = c;
	} else if (!( ( (buff->data[prevPoz] == '\r') || (buff->data[prevPoz] == '\n')) && (buff->data[buff->pozWrite] == ' ') )) { // for spaces at the beginning of the line
		/* increment counters */
		buff->usedSpace++;
		buff->pozWrite = bufferNextPosition(buff, buff->pozWrite);
	}
	
	return BUFFER_SUCCESS;
}

int bufferReadChar (struct T_buffer *buff, char *c) {
	if ( bufferIsEmpty(buff) )	{
		return BUFFER_ERROR;
	}
	*c = buff->data[buff->pozRead];
	buff->pozRead = bufferNextPosition(buff, buff->pozRead);
	buff->usedSpace--;
	return BUFFER_SUCCESS;
}

int bufferReadWord (struct T_buffer *buff, char *c) {
	int i = 0;
	char c_pom, status = 1;

	if (!bufferIsLineReady(buff)) {
		return BUFFER_NO_LINES;
	}
	
	while ( (!bufferIsEmpty(buff))) {
		//Naèti znak a kontroluj chyby
		if ( bufferReadChar( buff, &c_pom ) == BUFFER_ERROR ) {
			return BUFFER_ERROR;
		}
		
		if ( (c_pom == ' ') && (status == 1) ) {
			//pokud pøišla mezera a nejsme uvnitø složených závorek -> ukonèi
			c[i] = NULL;
			return BUFFER_SUCCESS;
		} else if ( (c_pom == '\r') || (c_pom == '\n') )  {
			//pokud pøišel konec øádku -> ukonèi
			buff->linesCount--;
			c[i] = NULL;
			return BUFFER_LINE_END;
		} else {
			//pokud pøišly uvozovky
			c[i] = c_pom;
			if ( c_pom == '"' ) {
				status = 1-status;
			}
			i++;
		}
	}
	return BUFFER_SUCCESS;
}

bool bufferIsEmpty (struct T_buffer *buff) {
	return (buff->usedSpace == 0);
}

bool bufferIsFull (struct T_buffer *buff) {
	return (buff->usedSpace >= buff->size);
}

bool bufferIsLineReady (struct T_buffer *buff) {
	return (buff->linesCount > 0);
}