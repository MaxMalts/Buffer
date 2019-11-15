#pragma once

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