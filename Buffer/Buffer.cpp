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

	if (buf->cursor >= buf->size) {
		return EOB;
	}
	return buf->str[buf->cursor++];
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

	char curCh = Bgetc(buf);

	while (!IsOneOfChars(curCh, chars)) {

		if (curCh == EOB) {
			return 1;
		}

		str[strCursor] = curCh;
		strCursor++;

		curCh = Bgetc(buf);
	}
	buf->cursor--;

	return 0;
}


/**
*	Устанавливает курсор в буфере в соответствии со значениями offset и origin
*
*	@param buf Буфер
*	@param[in] offset Сдвиг относительно позиции, соответствуйщей origin
*	@param[in] origin Начальная позиция: BSEEK_SET - начало буфера;\
 BSEEK_CUR - текущая позиция курсора; BSEEK_END - конец буфера
*
*	@return 1 - некорректное значение origin; 2 - курсор выходит за границы буфера;\
 0 - все прошло нормально
*/

int Bseek(buf_t* buf, int offset, int origin) {
	assert(buf != NULL);
	
	int newCursorPos = 0;
	switch (origin) {
	case BSEEK_SET:
		newCursorPos = offset;
		break;
	case BSEEK_CUR:
		newCursorPos = buf->cursor + offset;
		break;
	case BSEEK_END:
		newCursorPos = buf->lastChar + 1 + offset;
		break;
	default:
		return 1;
	}

	if (newCursorPos<0 || newCursorPos>buf->size) {
		return 2;
	}

	buf->cursor = newCursorPos;

	return 0;
}


/**
*	Возвращает текущее положение курсора в буфере
*
*	@param[in] buf Буфер
*
*	@return Текущее положение курсора в буфере
*/

int Btell(buf_t* buf) {
	assert(buf != NULL);

	return buf->cursor;
}


/*
*	Увеличивает размер буфера
*
*	@param buf Буфер
*	@param[in] newSize Новый размер
*
*	@return 1 - не удалось увеличить; 2 - буфер находится не в режиме записи;
 0 - все прошло нормально
*/

int IncreaseBuf(buf_t* buf, const int newSize) {
	assert(buf != NULL);

	if (buf->mode != 'w') {
		return 2;
	}

	char* newStr = (char*)realloc(buf->str, newSize);
	if (newStr == NULL) {
		return 1;
	}
	buf->str = newStr;
	memset(&buf->str[buf->size], 0, newSize - buf->size);
	buf->size = newSize;

	return 0;
}

/**
*	Удаляет буфер. Если он был в режиме чтения, то память со строкой не освобождается!
*
*	@param[in] buf Буфер
*
*	@return 1 - некорректный буфер на входе; 0 - все прошло нормально
*/

int BufDestructor(buf_t* buf) {
	assert(buf != NULL);

	if (buf->mode == 'r') {
		buf->str = NULL;
		buf->cursor = 0;
		buf->size = 0;
		buf->mode = 0;
		buf->lastChar = 0;
	}
	else if (buf->mode == 'w') {
		free(buf->str);
		buf->cursor = 0;
		buf->size = 0;
		buf->mode = 0;
		buf->lastChar = 0;
	}
	else {
		return 1;
	}

	return 0;
}