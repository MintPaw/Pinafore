#include <stdlib.h>
#include <string.h>

char *stringClone(const char *str);
bool streq(const char *str1, const char *str2);

#define ArrayLength(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

char *stringClone(const char *str) {
	char *ret = (char *)malloc((strlen(str)+1) * sizeof(char));
	strcpy(ret, str);
	return ret;
}

bool streq(const char *str1, const char *str2) {
	if (!str1 || !str2) return false;
	return strcmp(str1, str2) == 0;
}
