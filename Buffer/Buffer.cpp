#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "buffer.h"



/*  Не для пользователя
*	Пересчитывает последний символ исходя из текущего положения курсора
*
*	@param buf Буфер
*/

void RecalcLastChar(buf_t* buf) {
	assert(buf != NULL);

	if (buf->cursor > buf->lastChar) {
		buf->lastChar = buf->cursor;
	}
}


/**
*	Создает буфер в режиме чтения
*
*	@param[in] mode Режим (должен быть 'r')
*	@param str Строка, которую кладем в буфер
*	@param[in] strSize Размер строки (буфера)
*	@param[out] err Код ошибки: 1 - mode != 'r'; 0 - все прошло нормально
*
*	@return Созданный буфер (в случае ошибки равен ERR_BUF).
*/

buf_t BufConstructor(const char mode, char* str, const int strSize, int* err) {

	if (mode != 'r') {
		if (err != NULL) {
			*err = 1;
		}
		buf_t errBuf = ERR_BUF;
		return errBuf;
	}
	
	buf_t buf = {};

	buf.str = str;
	buf.cursor = 0;
	buf.size = strSize;
	buf.mode = 'r';
	buf.lastChar = strSize - 1;
	
	*err = 0;
	return buf;
}


/**
*	Создает буфер в режиме записи
*
*	@param[in] mode Режим (должен быть 'w')
*	@param[out] err Код ошибки: 1 - mode != 'w'; 0 - все прошло нормально
*
*	@return Созданный буфер (в случае ошибки равен ERR_BUF).
*/

buf_t BufConstructor(const char mode, int* err) {

	const int begSize = 10;

	if (mode != 'w') {
		if (err != NULL) {
			*err = 1;
		}
		buf_t errBuf = ERR_BUF;
		return errBuf;
	}

	buf_t buf = {};

	buf.str = (char*)calloc(begSize, sizeof(char));
	buf.cursor = 0;
	buf.size = begSize;
	buf.mode = 'w';
	buf.lastChar = 0;

	return buf;
}


/**
*	Пишет символ в буфер
*
*	@param buf Буфер
*	@param[in] ch Символ
*
*	@return 1 - буфер не в режиме записи; 2 - не удалось увеличить размер буфера;\
 0 - все прошло нормально
*/

int Bputc(buf_t* buf, const char ch) {
	assert(buf != NULL);

	if (buf->mode != 'w') {
		return 1;
	}

	if (buf->cursor >= buf->size - 1) {
		if (realloc(buf->str, buf->size * 2) == NULL) {
			return 2;
		}
	}

	buf->str[buf->cursor++] = ch;

	RecalcLastChar(buf);
	

	return 0;
}


/**
*	Читает символ из буфера
*
*	@param[in] buf Буфер
*
*	@return Считанный символ. При достижении конца буфера, возвращает EOB.\
 Если вернулось -2, то буфер не в режиме чтения.
*/

char Bgetc(buf_t* buf) {
	assert(buf != NULL);

	if (buf->mode != 'r') {
		return -2;
	}

	if (buf->cursor >= buf->size - 1) {
		return EOB;
	}
	return buf->str[buf->cursor];
}


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
	int printLen = sprintf(tesmpStr, format, args...);

	if (printLen < 0) {
		return printLen;
	}

	if (buf->cursor + printLen >= buf->size) {
		if (realloc(buf->str, (buf->size + printLen) * 2) == NULL) {
			return -401;
		}
		strncpy(&buf->str[buf->cursor], tempStr, printLen);
	}

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
	char* newFormat = (char*)calloc(formatLen + 3, sizeof(char));
	sprintf(newFormat, "%s%%n", format);

	int charsRead = 0;
	int scanRet = sscanf(buf->str[buf->cursor], newFormat, args..., &charsRead);
	if (scanRet != sizeof...(args)) {
		return -2;
	}

	return scanRet;
}


/**
*	Пишет строку в конец буфера
*
*	@param buf
*	@param[in] str
*
*	@return 1 - буфер не в режиме записи; 2 - проблема при дописывании; 0 - все прошло нормально
*/

int Bufcat(buf_t* buf, const char* str) {
	assert(buf != NULL);
	assert(str != NULL);

	if (buf->mode != 'w') {
		return 1;
	}

	int SLen = sprintf(&buf->str[buf->cursor], "%s", str);

	if (SLen < 0) {
		return 2;
	}

	buf->cursor += SLen;
	buf->size += SLen;
	buf->str[buf->cursor] = '\0';

	return 0;
}


/**
*	Прверяет, является ли символ одним из данных
*
*	@param[in] ch Проверяемый символ
*	@param[in] chars Массив символов
*
*	@return 0 (false) - не является; 1 (true) - является
*/

int IsOneOfChars(const char ch, const char* chars) {
	assert(chars != NULL);

	for (int i = 0; i < strlen(chars); i++) {
		if (ch == chars[i]) {
			return 1;
		}
	}
	return 0;
}


/**
*	Читает из буфера до одного из заданного символов
*
*	@param[out] str Строка, в которую прочитается
*	@param[in] buf Буфер
*	@param[in] chars Массив конечных символов
*
*	@return 1 - символ не найден, курсор остается на том же месте;\
 0 - все прошло нормально
*/

int ReadToChar(char* str, buf_t* buf, const char* chars) {
	assert(str != NULL);
	assert(buf != NULL);
	assert(chars != NULL);

	int NChars = strlen(chars);

	int cursorRecover = buf->cursor;

	int strCursor = 0;

	char curCh = buf->str[buf->cursor];

	while (!IsOneOfChars(curCh, chars)) {

		if (buf->cursor > buf->size) {
			buf->cursor = cursorRecover;
			return 1;
		}

		str[strCursor] = curCh;
		buf->cursor++;
		strCursor++;

		curCh = buf->str[buf->cursor];
	}

	return 0;
}