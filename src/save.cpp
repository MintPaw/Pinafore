struct Save {
	void *memory;
	int position;
	int size;

	void pushBytes(void *bytes, int bytesNum);
	void pushString(const char *str);
	void pushInt(int32_t num);
	void pushFloat(float num);

	void popBytes(void *dest, int destNum);
	void popString(char *str);
	int32_t popInt();
	float popFloat();

	void destroy();
};

void initSave(Save *save, void *memory=NULL, int size=32);

void initSave(Save *save, void *memory, int size) {
	memset(save, 0, sizeof(Save));
	if (memory) {
		save->size = size;
		save->memory = memory;
	} else {
		save->size = 32;
		save->memory = malloc(save->size);
	}
};

void Save::pushBytes(void *bytes, int bytesNum) {
	Save *save = this;

	if (save->position + bytesNum > save->size) {
		save->memory = realloc(save->memory, save->size * 2);
		save->size *= 2;
	}

	memcpy(((char *)save->memory) + save->position, bytes, bytesNum);
	save->position += bytesNum;
}

void Save::pushString(const char *str) {
	this->pushBytes((void *)str, strlen(str));
	char zero = 0;
	this->pushBytes(&zero, 1);
}

void Save::pushInt(int32_t num) {
	this->pushBytes(&num, sizeof(int32_t));
}

void Save::pushFloat(float num) {
	this->pushBytes(&num, sizeof(float));
}

void Save::popBytes(void *dest, int destNum) {
	Save *save = this;

	if (save->position + destNum > save->size) {
		printf("Out of bytes to read\n");
		Assert(0);
	}

	memcpy(dest, ((char *)save->memory) + save->position, destNum);

	save->position += destNum;
}

void Save::popString(char *str) {
	Save *save = this;

	int len = strnlen(((char *)save->memory) + save->position, save->size - save->position);
	save->popBytes(str, len+1);
}

int32_t Save::popInt() {
	int32_t num = 0;
	this->popBytes(&num, sizeof(int32_t));
	return num;
}

float Save::popFloat() {
	float num = 0;
	this->popBytes(&num, sizeof(float));
	return num;
}

void Save::destroy() {
	free(this->memory);
}
