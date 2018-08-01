
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>

int getFileSize(const char *path) {
	struct stat st = {0};
	stat(path, &st);
	return st.st_size + 1;
}

int stringFromFile(const char *restrict path, char *dest, uint32_t maxWrite) {
	if (!dest || !path || maxWrite < 1) return 0;
	FILE *fp = fopen(path, "r");
	if (!fp) {
		printf("error: could not open file \"%s\"\n", path);
		return 0;
	}
	int i = 0;
	for (char c; i < maxWrite; i++) {
		c = fgetc(fp);
		if (c == EOF) break;
		dest[i] = c;
	}
	dest[i] = '\0';
	fclose(fp);
	return i;
}
