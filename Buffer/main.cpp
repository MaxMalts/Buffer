#include <stdio.h>
#include "buffer.h"

int main() {
	char str[] = "This is a test string\nto test a buffer.\nThere are numbers now: 1, 2, 3, 6, 1234, 43, -43, ...";

	int err = 0;
	buf_t buffer = BufConstructor('r', str, sizeof(str), &err);

	char c = Bgetc(&buffer);
	c = Bgetc(&buffer);
	c = Bgetc(&buffer);
	c = Bgetc(&buffer);
	c = Bgetc(&buffer);

	char str1[100] = "";
	Bscanf(&buffer, "%s", str1);
	
	ReadToChar(str1, &buffer, "1234567890");

	int a[3] = {};
	Bscanf(&buffer, "%d, %d, %d", &a[0], &a[1], &a[2]);

	Bseek(&buffer, -2, BSEEK_END);

	c = Bgetc(&buffer);
	c = Bgetc(&buffer);
	c = Bgetc(&buffer);
	c = Bgetc(&buffer);

	BufDestructor(&buffer);


	buffer = BufConstructor('w', &err);

	Bprintf(&buffer, "%s %d, %d\n", "test", 4, 67);
	Bprintf(&buffer, "%s, %d", "test", 67);

	BufDestructor(&buffer);

	return 0;
}