#include <stdio.h>

static class Util {
public:
	static void Test() {
		printf("Test");
	}

	static void TrimChars(char * pChar,int length) {
		for (int i = 0; i < length; i++) {
			if ((int)pChar[i] < 0 || (int)pChar[i] > 255) {
				pChar[i] = 0x00;
			}
			printf("%c", pChar[i]);
		}
	}
};