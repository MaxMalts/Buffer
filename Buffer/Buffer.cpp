#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define EOB -1   //������� ����� ������
#define ERR_BUF {NULL, 0, -1, 0}

/**
*	������ � ��������
*/

struct buf_t {
	char* str = {};
	int cursor = 0;
	int size = 0;
	char mode = 0;
	int lastChar = 0;
};


void RecalcLastChar(buf_t* buf) {
	assert(buf != NULL);

	if (buf->cursor > buf->lastChar) {
		buf->lastChar = buf->cursor;
	}
}

buf_t BufConstructor(const char mode, char* str, const int strSize, int* err = NULL) {

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
	

	return buf;
}

buf_t BufConstructor(const char mode, int* err = NULL) {

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

char Bgetc(buf_t* buf) {
	assert(buf != NULL);

	if (buf->cursor >= buf->size - 1) {
		return EOB;
	}
	return buf->str[buf->cursor];
}

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
*	����� ������ � ����� ������
*
*	@param buf
*	@param[in] str
*
*	@return 1 - ������ �� � ������ ������; 2 - �������� ��� �����������; 0 - ��� ������ ���������
*/

int bufcat(buf_t* buf, const char* str) {
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

///**
//*	������� ������ ������ � ���������� ���
//*
//*	@param buf �����
//*
//*	@return ������; -1 - ������ ������������
//*/
//
//int RecalcBufSize(buf_t* buf) {
//	assert(buf != NULL);
//
//	if (buf->mode == 'r') {
//		int res = strlen(buf->str);
//		buf->size = res;
//		return res;
//	}
//	if (buf->mode == 'w') {
//		return buf->size;
//	}
//
//	return -1;
//}


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