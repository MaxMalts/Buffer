#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "buffer.h"



/*  �� ��� ������������
*	������������� ��������� ������ ������ �� �������� ��������� �������
*
*	@param buf �����
*/

void RecalcLastChar(buf_t* buf) {
	assert(buf != NULL);

	if (buf->cursor > buf->lastChar) {
		buf->lastChar = buf->cursor;
	}
}


/**
*	������� ����� � ������ ������
*
*	@param[in] mode ����� (������ ���� 'r')
*	@param str ������, ������� ������ � �����
*	@param[in] strSize ������ ������ (������)
*	@param[out] err ��� ������: 1 - mode != 'r'; 0 - ��� ������ ���������
*
*	@return ��������� ����� (� ������ ������ ����� ERR_BUF).
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
*	������� ����� � ������ ������
*
*	@param[in] mode ����� (������ ���� 'w')
*	@param[out] err ��� ������: 1 - mode != 'w'; 0 - ��� ������ ���������
*
*	@return ��������� ����� (� ������ ������ ����� ERR_BUF).
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
*	����� ������ � �����
*
*	@param buf �����
*	@param[in] ch ������
*
*	@return 1 - ����� �� � ������ ������; 2 - �� ������� ��������� ������ ������;\
 0 - ��� ������ ���������
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
*	������ ������ �� ������
*
*	@param[in] buf �����
*
*	@return ��������� ������. ��� ���������� ����� ������, ���������� EOB.\
 ���� ��������� -2, �� ����� �� � ������ ������.
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
*	����������� scanf ������� ��� ������
*
*	@param[in] buf �����
*	@param[in] format ������ �������
*	@param[in] args ���������
*
*	@return � ������ ������ ���������� ���������� ���������� ��������.\
 ���� �������� ������������, �� ��������� ��������� ������:\
 -403 - ����� �� � ������ ������; -401 - �� ������� ��������� ������ ������\
 ���. - ������ ��� ������, ��������� ������������ ������� ����������
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
*	����������� scanf ������� ��� ������
*
*	@param[in] buf �����
*	@param[in] format ������ �������
*	@param[in] args ���������
*
*	@return � ������ ������ ���������� ���������� ����������� ����������.\
 ���� �������� ������������, �� ��������� ��������� ������:\
 -1 - ����� �� � ������ ������; -2 - ������ ��� ������,\
 ��������� ������������ ������� ����������
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
*	����� ������ � ����� ������
*
*	@param buf
*	@param[in] str
*
*	@return 1 - ����� �� � ������ ������; 2 - �������� ��� �����������; 0 - ��� ������ ���������
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
*	��������, �������� �� ������ ����� �� ������
*
*	@param[in] ch ����������� ������
*	@param[in] chars ������ ��������
*
*	@return 0 (false) - �� ��������; 1 (true) - ��������
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
*	������ �� ������ �� ������ �� ��������� ��������
*
*	@param[out] str ������, � ������� �����������
*	@param[in] buf �����
*	@param[in] chars ������ �������� ��������
*
*	@return 1 - ������ �� ������, ������ �������� �� ��� �� �����;\
 0 - ��� ������ ���������
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