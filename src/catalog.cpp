#define CATALOG_VARS_LIMIT 256
#define CATALOG_LINE_LIMIT 1024
#define CATALOG_NAME_LIMIT 256

enum CatalogType {
	CATALOG_INT,
	CATALOG_FLOAT,
	CATALOG_STRING,
};

struct CatalogVar {
	CatalogType type;
	char *name;
	void *ptr;
};

struct Catalog {
	char path[256];

	CatalogVar vars[CATALOG_VARS_LIMIT];
	int varsNum;
};

void initCatalog(Catalog *cat, const char *path);
void attachInt(Catalog *cat, int *ptr, const char *name);
void attachFloat(Catalog *cat, float *ptr, const char *name);
void attachString(Catalog *cat, char *ptr, const char *name);

CatalogVar *getCatalogVar(Catalog *cat, const char *name);
void pullCatalog(Catalog *cat);
void pullCatalogByName(const char *name);

void initCatalog(Catalog *cat, const char *path) {
	memset(cat, 0, sizeof(Catalog));
	strcpy(cat->path, path);
}

void attachInt(Catalog *cat, int *ptr, const char *name) {
	if (cat->varsNum+1 == CATALOG_VARS_LIMIT) {
		printf("Too many vars!\n");
		return;
	}

	CatalogVar *var = &cat->vars[cat->varsNum++];
	var->name = stringClone(name);
	var->ptr = ptr;
	var->type = CATALOG_INT;
}

void attachFloat(Catalog *cat, float *ptr, const char *name) {
	if (cat->varsNum+1 == CATALOG_VARS_LIMIT) {
		printf("Too many vars!\n");
		return;
	}

	CatalogVar *var = &cat->vars[cat->varsNum++];
	var->name = stringClone(name);
	var->ptr = ptr;
	var->type = CATALOG_FLOAT;
}

void attachString(Catalog *cat, char *ptr, const char *name) {
	if (cat->varsNum+1 == CATALOG_VARS_LIMIT) {
		printf("Too many vars!\n");
		return;
	}

	CatalogVar *var = &cat->vars[cat->varsNum++];
	var->name = stringClone(name);
	var->ptr = ptr;
	var->type = CATALOG_STRING;
}

CatalogVar *getCatalogVar(Catalog *cat, const char *name) {
	for (int i = 0; i < cat->varsNum; i++) {
		CatalogVar *var = &cat->vars[i];
		if (streq(var->name, name)) return var;
	}

	return NULL;
}

void pullCatalog(Catalog *cat) {
	int dataLen;
	char *data = (char *)readFile(cat->path, &dataLen);
	if (!data) {
		printf("Catalog file %s missing!\n", cat->path);
		return;
	}

	char *curPos = data;

	for (;;) {
		char *lineEnd = strchr(curPos, '\n');
		if (!lineEnd) {
			char *dataEnd = data + strlen(data);
			if (curPos < dataEnd) lineEnd = dataEnd;
			else break;
		}

		char line[CATALOG_LINE_LIMIT];
		int lineLen = lineEnd - curPos;
		if (lineLen == 0) {
			curPos++;
			continue;
		}
		strncpy(line, curPos, lineLen);
		line[lineLen] = '\0';
		curPos += lineLen+1;

		if (line[0] == '/' && line[1] == '/') continue;

		char name[CATALOG_NAME_LIMIT];
		char *nameEnd = strchr(line, ' ');
		if (!nameEnd) {
			printf("Can't find end of name!\n");
			Assert(0);
		}
		int nameLen = nameEnd - line;
		strncpy(name, line, nameLen);
		name[nameLen] = '\0';

		char *valueStr = line + nameLen;
		while(
			valueStr[0] == ' ' ||
			valueStr[0] == '\t'
		) valueStr++;

		CatalogVar *var = getCatalogVar(cat, name);
		if (!var) {
			printf("Var named \"%s\" is in catalog, but not attached\n", name);
			continue;
		}

		if (var->type == CATALOG_INT) {
			if (valueStr[0] == '0' && valueStr[1] == 'x') sscanf(valueStr, "0x%x", (int *)var->ptr);
			else sscanf(valueStr, "%d", (int *)var->ptr);
		} else if (var->type == CATALOG_FLOAT) {
			sscanf(valueStr, "%f", (float *)var->ptr);
		} else if (var->type == CATALOG_STRING) {
			strcpy((char *)var->ptr, valueStr);
		}

	}

	free(data);
}
