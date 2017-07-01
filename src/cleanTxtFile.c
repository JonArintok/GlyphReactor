#include "fileTools.h"
#include "cleanTxtFile.h"

int cleanTxtFile(const char *txtPath, char *chars, int writeLimit) {
	int readLimit = stringFromFile(txtPath, chars, writeLimit);
	int readPos = 0;
	int writPos = 0;
	int writePosInWord = 0;
	while (writPos < writeLimit) {
		if (chars[readPos] == '\0' || readPos > readLimit) {
			chars[writPos] = '\0';
			break;
		}
		if (chars[readPos] < '!' || chars[readPos] > '~') {
			readPos++;
			if (chars[writPos-1] != delim) {
				chars[writPos] = delim;
				writPos++;
				writePosInWord = 0;
			}
			continue;
		}
		chars[writPos] = chars[readPos];
		readPos++;
		writPos++;
		writePosInWord++;
		if (writePosInWord >= maxWordSize) {
			// this overwrites a character, but it's just to prevent memory errors
			chars[writPos] = delim;
			writPos++;
			writePosInWord = 0;
		}
	}
	return writPos;
}
