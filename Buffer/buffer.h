#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define EOB -1   //признак конца буфера
#define ERR_BUF {NULL, 0, -1, 0}    //буфер - ошибка

#define BSEEK_SET 0
#define BSEEK_CUR 1
#define BSEEK_END 2

/**
*	Буфер (строка с курсором)
*/

struct buf_t {
	char* str = {};
	int cursor = 0;
	int size = 0;
	char mode = 0;
	int lastChar = 0;
};



buf_t BufConstructor(const char mode, char* str, const int strSize, int* err = NULL);

buf_t BufConstructor(const char mode, int* err = NULL);

int Bputc(buf_t* buf, const char ch);

char Bgetc(buf_t* buf);

template<typename... args_t>
int Bprintf(buf_t* buf, const char* format, args_t... args);

template<typename... args_t>
int Bscanf(buf_t* buf, const char* format, args_t... args);

int Bufcat(buf_t* buf, const char* str);

int IsOneOfChars(const char ch, const char* chars);

int ReadToChar(char* str, buf_t* buf, const char* chars);

int Bseek(buf_t* buf, int offset, int origin);

int Btell(buf_t* buf);

int BufDestructor(buf_t* buf);





int IncreaseBuf(buf_t* buf, const int newSize);

void RecalcLastChar(buf_t* buf);

/**
*	Аналогичная scanf функция для буфера
*
*	@param[in] buf Буфер
*	@param[in] format Строка фармата
*	@param[in] args Аргументы
*
*	@return В случае успеха возвращает количество записанныз символов.\
 Если значение отрицательно, то произошли следующие ошибки:\
 -403 - буфер не в режиме записи; -401 - не удалось увеличить размер буфера\
 ост. - ошибка при записи, проверьте корректность входных параметров
*/

template<typename... args_t>
int Bprintf(buf_t* buf, const char* format, args_t... args) {
	assert(buf != NULL);
	assert(format != NULL);

	const int stringMaxSize = 100000;

	if (buf->mode != 'w') {
		return -403;
	}

	char tempStr[stringMaxSize] = "";
	int printLen = sprintf(tempStr, format, args...);

	if (printLen < 0) {
		return printLen;
	}
	
	if (buf->cursor + printLen >= buf->size) {
		if (IncreaseBuf(buf, (buf->size + printLen) * 2) != 0) {
			return -401;
		}
	}

	strncpy(&buf->str[buf->cursor], tempStr, printLen);
	buf->cursor += printLen;

	RecalcLastChar(buf);

	return printLen;
}


/**
*	Аналогичная scanf функция для буфера
*
*	@param[in] buf Буфер
*	@param[in] format Строка фармата
*	@param[in] args Аргументы
*
*	@return В случае успеха возвращает количество прочитанных параметров.\
 Если значение отрицательно, то произошли следующие ошибки:\
 -1 - буфер не в режиме чтения; -2 - ошибка при чтении,\
 проверьте корректность входных параметров
*/

template<typename... args_t>
int Bscanf(buf_t* buf, const char* format, args_t... args) {
	assert(buf != NULL);
	assert(format != NULL);

	if (buf->mode != 'r') {
		return -1;
	}

	int formatLen = strlen(format);
	const char* newFormat = (char*)calloc(formatLen + 3, sizeof(char));
	sprintf((char*)newFormat, "%s%%n", format);

	int charsRead = 0;
	int scanRet = sscanf(&buf->str[buf->cursor], newFormat, args..., &charsRead);
	if (scanRet != sizeof...(args)) {
		return -2;
	}

	buf->cursor += charsRead;

	return scanRet;
}