#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GetAttrib(programName, attribName) programName.attribName = glGetAttribLocation(programName.program, #attribName) 
#define GetUniform(programName, uniformName) programName.uniformName = glGetUniformLocation(programName.program, #uniformName) 
struct RenderProps;
struct Texture;

void initRenderer(int screenWidth, int screenHeight);
GLuint buildShader(char *vertPath, char *fragPath);

void setShaderProgram(GLuint program);
void setArrayBuffer(GLuint buffer);
void changeArrayBuffer(GLuint glBuffer, float x, float y, float width, float height);
GLuint genGlArrayBuffer(float x, float y, float width, float height);
void setTexture(GLuint texture, int slot = 0);
void setFramebufferTexture(GLuint texture);
void setFramebuffer(GLuint framebuffer);
void setViewport(float x, float y, float w, float h);
void destroyTexture(Texture *tex);

Texture *uploadPngTexturePath(const char *path);
Texture *uploadPngTexture(void *data, int size);
Texture *uploadTexture(void *data, int width, int height);

void clearRenderer(float r=1, float g=1, float b=1, float a=1);
void setTargetTexture(Texture *texture);
void revertTargetTexture();
RenderProps newRenderProps();
void drawCircle(float x, float y, float radius, int colour);
void drawRect(float x, float y, float width, float height, int colour);
void drawTiles(Texture *srcTexture, int destWidth, int destHeight, int tileWidth, int tileHeight, int tilesWide, int tilesHigh, int *tiles);
void drawTexture(Texture *texture, RenderProps *addedProps);
void drawOneBppBitmap(void *bitmapData, RenderProps *addedProps);

void checkGlError(int lineNum);

#define CheckGlError() checkGlError(__LINE__);

struct RenderProps {
	float x;
	float y;
	float scaleX;
	float scaleY;
	float alpha;
	float rotation;
	int tint;

	float pivotX;
	float pivotY;

	float scrollFactor;

	int srcX;
	int srcY;
	int srcWidth;
	int srcHeight;
	int localX;
	int localY;
};

struct Texture {
	GLuint id;
	int width;
	int height;
};

struct RectProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_colour;
	GLuint u_projection;
};

struct CircleProgram {
	GLuint program;
	GLuint a_position;
	GLuint a_texCoord;
	GLuint u_colour;
	GLuint u_projection;
};

struct TilemapProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_tilesetTexture;
	GLuint u_tilemapTexture;
	GLuint u_tilesetSize;
	GLuint u_tilemapSize;
	GLuint u_resultSize;
	GLuint u_pixelRatio;
};

struct DefaultProgram {
	GLuint program;
	GLuint a_position;
	GLuint a_texCoord;
	GLuint u_matrix;
	GLuint u_uv;
	GLuint u_tint;
	GLuint u_alpha;
};

struct OneBppProgram {
	GLuint program;
	GLuint a_position;
	GLuint a_texCoord;
	GLuint u_matrix;
	GLuint u_uv;
	GLuint u_tint;
	GLuint u_alpha;
};

struct Renderer {
	int errorCount;
	int screenWidth;
	int screenHeight;
	Rect camera;

	RectProgram rectProgram;
	CircleProgram circleProgram;
	DefaultProgram defaultProgram;
	TilemapProgram tilemapProgram;
	OneBppProgram oneBppProgram;

	Texture *currentTargetTexture;
	Texture *prevTargetTexture;

	GLuint tempVerts;
	GLuint tempTexCoords;
	GLuint textureFramebuffer;

	GLuint curShaderProgram;
	GLuint curArrayBuffer;
	GLuint currentTexture;
	GLuint currentFramebuffer;
	GLuint currentFramebufferTexture;
	Rect currentViewport;
};

Renderer *renderer;

/// End header

void initRenderer(int screenWidth, int screenHeight) {
	printf("Initing renderer\n");

	renderer = (Renderer *)malloc(sizeof(Renderer));
	memset(renderer, 0, sizeof(Renderer));
	renderer->screenWidth = screenWidth;
	renderer->screenHeight = screenHeight;
	stbi_set_flip_vertically_on_load(true);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE);
	CheckGlError();

#ifdef GL_CORE
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif

	renderer->rectProgram.program = buildShader("assets/shaders/rect.vert", "assets/shaders/rect.frag");
	GetAttrib(renderer->rectProgram, a_position);
	GetUniform(renderer->rectProgram, u_colour);
	GetUniform(renderer->rectProgram, u_projection);

	renderer->circleProgram.program = buildShader("assets/shaders/circle.vert", "assets/shaders/circle.frag");
	GetAttrib(renderer->circleProgram, a_position);
	GetAttrib(renderer->circleProgram, a_texCoord);
	GetUniform(renderer->circleProgram, u_colour);
	GetUniform(renderer->circleProgram, u_projection);

	renderer->defaultProgram.program = buildShader("assets/shaders/default.vert", "assets/shaders/default.frag");
	GetAttrib(renderer->defaultProgram, a_position);
	GetAttrib(renderer->defaultProgram, a_texCoord);
	GetUniform(renderer->defaultProgram, u_matrix);
	GetUniform(renderer->defaultProgram, u_uv);
	GetUniform(renderer->defaultProgram, u_tint);
	GetUniform(renderer->defaultProgram, u_alpha);

	renderer->tilemapProgram.program = buildShader("assets/shaders/tilemap.vert", "assets/shaders/tilemap.frag");
	GetAttrib(renderer->tilemapProgram, a_position);
	GetUniform(renderer->tilemapProgram, u_matrix);
	GetUniform(renderer->tilemapProgram, u_tilesetTexture);
	GetUniform(renderer->tilemapProgram, u_tilemapTexture);
	GetUniform(renderer->tilemapProgram, u_tilesetSize);
	GetUniform(renderer->tilemapProgram, u_tilemapSize);
	GetUniform(renderer->tilemapProgram, u_resultSize);
	GetUniform(renderer->tilemapProgram, u_pixelRatio);

	renderer->oneBppProgram.program = buildShader("assets/shaders/oneBpp.vert", "assets/shaders/oneBpp.frag");
	GetAttrib(renderer->oneBppProgram, a_position);
	GetAttrib(renderer->oneBppProgram, a_texCoord);
	GetUniform(renderer->oneBppProgram, u_matrix);
	GetUniform(renderer->oneBppProgram, u_uv);
	GetUniform(renderer->oneBppProgram, u_tint);
	GetUniform(renderer->oneBppProgram, u_alpha);

	CheckGlError();

	renderer->tempVerts = genGlArrayBuffer(0, 0, 0, 0);
	renderer->tempTexCoords = genGlArrayBuffer(0, 0, 0, 0);
	glGenFramebuffers(1, &renderer->textureFramebuffer);
	CheckGlError();

	setTargetTexture(0);
}

GLuint buildShader(char *vertPath, char *fragPath) {
	char *vertSrc;
	char *fragSrc;
	readFile(vertPath, (void **)&vertSrc);
	readFile(fragPath, (void **)&fragSrc);

#ifdef GL_ES
	const char *versionLine = "#version 300 es\n";
#else
	const char *versionLine = "#version 330\n";
#endif

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	char *realVert = (char *)malloc((strlen(vertSrc) + strlen(versionLine) + 1) * sizeof(char));
	strcpy(realVert, versionLine);
	strcat(realVert, vertSrc);
	int vertLen = strlen(realVert);
	glShaderSource(vert, 1, (const char **)&realVert, &vertLen);
	glCompileShader(vert);
	free(realVert);

	static const int errLogLimit = 1024;
	char errLog[errLogLimit];
	int errLogNum;

	int vertReturn;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vertReturn);
	if (!vertReturn) {
		glGetShaderInfoLog(vert, errLogLimit, &errLogNum, errLog);
		printf("Vertex result is: %d\nError(%d):\n%s\n", vertReturn, errLogNum, errLog);
	}

	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	char *realFrag = (char *)malloc((strlen(fragSrc) + strlen(versionLine) + 1) * sizeof(char));
	strcpy(realFrag, versionLine);
	strcat(realFrag, fragSrc);
	int fragLen = strlen(realFrag);
	glShaderSource(frag, 1, (const char **)&realFrag, &fragLen);
	glCompileShader(frag);
	free(realFrag);

	int fragReturn;
	glGetShaderiv(frag, GL_COMPILE_STATUS, &fragReturn);
	if (!fragReturn) {
		glGetShaderInfoLog(frag, errLogLimit, &errLogNum, errLog);
		printf("Fragment result is: %d\nError:\n%s\n", fragReturn, errLog);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	int progReturn;
	glGetProgramiv(program, GL_LINK_STATUS, &progReturn);
	if (!progReturn) {
		glGetShaderInfoLog(program, errLogLimit, &errLogNum, errLog);
		printf("Program result is: %d\nError:\n%s\n", progReturn, errLog);
	}

	CheckGlError();

	free(vertSrc);
	free(fragSrc);
	return program;
}

void clearRenderer(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

RenderProps newRenderProps() {
	RenderProps props = {};
	props.scaleX = 1;
	props.scaleY = 1;
	props.alpha = 1;
	props.scrollFactor = 1;
	return props;
}

void drawRect(float x, float y, float width, float height, int colour) {
	setShaderProgram(renderer->rectProgram.program);
	CheckGlError();

	x -= renderer->camera.x;
	y -= renderer->camera.y;

	glEnableVertexAttribArray(renderer->rectProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, x, y, x+width, y+height);
	glVertexAttribPointer(renderer->rectProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	Matrix3 projection;
	projection.identity();
	projection.project(renderer->currentViewport.width, renderer->currentViewport.height);
	glUniformMatrix3fv(renderer->rectProgram.u_projection, 1, false, (float *)projection.data);

	glUniform4f(
		renderer->rectProgram.u_colour,
		((colour >> 16) & 0xff)/255.0,
		((colour >> 8) & 0xff)/255.0,
		(colour & 0xff)/255.0,
		((colour >> 24) & 0xff)/255.0
	);
	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);

	glDisableVertexAttribArray(renderer->rectProgram.a_position);

	CheckGlError();
}

void setTargetTexture(Texture *texture) {
	renderer->prevTargetTexture = renderer->currentTargetTexture;
	renderer->currentTargetTexture = texture;

	if (texture == NULL) {
		setFramebuffer(0);
		setViewport(0, 0, renderer->screenWidth, renderer->screenHeight);
	} else {
		setFramebuffer(renderer->textureFramebuffer);
		setFramebufferTexture(texture->id);
		setViewport(0, 0, texture->width, texture->height);
	}

	CheckGlError();
};

void revertTargetTexture() {
	setTargetTexture(renderer->prevTargetTexture);
}

void drawCircle(float x, float y, float radius, int colour) {
	setShaderProgram(renderer->circleProgram.program);
	CheckGlError();

	x -= radius/2;
	y -= radius/2;

	x -= renderer->camera.x;
	y -= renderer->camera.y;

	glEnableVertexAttribArray(renderer->circleProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, x, y, x+radius, y+radius);
	glVertexAttribPointer(renderer->circleProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	glEnableVertexAttribArray(renderer->circleProgram.a_texCoord);
	changeArrayBuffer(renderer->tempTexCoords, 0, 0, 1, 1);
	glVertexAttribPointer(renderer->circleProgram.a_texCoord, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	Matrix3 projection;
	projection.identity();
	projection.project(renderer->currentViewport.width, renderer->currentViewport.height);
	glUniformMatrix3fv(renderer->circleProgram.u_projection, 1, false, (float *)projection.data);

	glUniform4f(
		renderer->circleProgram.u_colour,
		((colour >> 16) & 0xff)/255.0,
		((colour >> 8) & 0xff)/255.0,
		(colour & 0xff)/255.0,
		((colour >> 24) & 0xff)/255.0
	);
	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);

	glDisableVertexAttribArray(renderer->circleProgram.a_position);
	glDisableVertexAttribArray(renderer->circleProgram.a_texCoord);

	CheckGlError();
}

void drawTexture(Texture *texture, RenderProps *addedProps) {
	RenderProps newProps = *addedProps;
	RenderProps *props = &newProps;
	if (props->srcWidth == 0) props->srcWidth = texture->width;
	if (props->srcHeight == 0) props->srcHeight = texture->height;

	setShaderProgram(renderer->defaultProgram.program);
	CheckGlError();

	glEnableVertexAttribArray(renderer->defaultProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, 0, 0, props->srcWidth, props->srcHeight);
	glVertexAttribPointer(renderer->defaultProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	float texX = (float)props->srcX/texture->width;
	float texY = (float)props->srcY/texture->height;
	float texW = (float)props->srcWidth/texture->width;
	float texH = (float)props->srcHeight/texture->height;
	changeArrayBuffer(renderer->tempTexCoords, texX, texY, texX + texW, texY + texH);
	glEnableVertexAttribArray(renderer->defaultProgram.a_texCoord);
	glVertexAttribPointer(renderer->defaultProgram.a_texCoord, 2, GL_FLOAT, false, 0, 0);
	CheckGlError();

	Point camOff = {renderer->camera.x*props->scrollFactor, renderer->camera.y*props->scrollFactor};

	Matrix3 matrix;
	matrix.identity();
	matrix.project(renderer->currentViewport.width, renderer->currentViewport.height);
	matrix.translate(props->x - camOff.x, props->y - camOff.y);

	matrix.translate(props->pivotX, props->pivotY);
	matrix.rotate(-props->rotation);
	matrix.scale(props->scaleX, props->scaleY);
	matrix.translate(-props->pivotX, -props->pivotY);

	matrix.translate(props->localX, props->localY);

	glUniformMatrix3fv(renderer->defaultProgram.u_matrix, 1, false, (float *)matrix.data);

	setTexture(texture->id);

	Matrix3 uv;
	uv.identity();
	uv.scale(1, -1); // Flip UVs to opengl format
	uv.translate(0, -1); // Flip UVs to opengl format
	glUniformMatrix3fv(renderer->defaultProgram.u_uv, 1, false, (float *)uv.data);

	glUniform4f(
		renderer->defaultProgram.u_tint,
		((props->tint >> 16) & 0xff)/255.0,
		((props->tint >> 8) & 0xff)/255.0,
		(props->tint & 0xff)/255.0,
		((props->tint >> 24) & 0xff)/255.0
	);

	glUniform1f(renderer->defaultProgram.u_alpha, props->alpha);

	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	CheckGlError();

	glDisableVertexAttribArray(renderer->defaultProgram.a_position);
	glDisableVertexAttribArray(renderer->defaultProgram.a_texCoord);
	CheckGlError();
}

void drawTiles(Texture *srcTexture, int destWidth, int destHeight, int tileWidth, int tileHeight, int tilesWide, int tilesHigh, int *tiles) {
	unsigned char *texData = (unsigned char *)malloc(tilesWide * tilesHigh * 4);

	memset(texData, 0, tilesWide * tilesHigh * 4);
	for (int i = 0; i < tilesWide * tilesHigh; i++) {
		texData[i*4 + 0] = ((tiles[i]) & 0xff);
		texData[i*4 + 1] = ((tiles[i] >> 8) & 0xff);

		/////
		// I probably don't need all these tileset slots just yet
		/////
		// texData[i*4 + 2] = ((tiles[i] >> 16) & 0xff);
		// texData[i*4 + 3] = ((tiles[i] >> 24) & 0xff);
		// printf("%d %d\n", texData[i*4 + 0], texData[i*4 + 1]);
	}

	Texture *tempTex = uploadTexture(texData, tilesWide, tilesHigh);
	CheckGlError();
	free(texData);

	/////
	// glCheckFramebufferStatus is really slow last I checked
	/////
	// if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("Framebuffer is not ready!\n");

	setShaderProgram(renderer->tilemapProgram.program);
	CheckGlError();

	glEnableVertexAttribArray(renderer->tilemapProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, 0, 0, destWidth, destHeight);
	glVertexAttribPointer(renderer->tilemapProgram.a_position, 2, GL_FLOAT, false, 0, 0);
	CheckGlError();

	Matrix3 projection;
	projection.identity();
	projection.project(renderer->currentViewport.width, renderer->currentViewport.height);
	glUniformMatrix3fv(renderer->tilemapProgram.u_matrix, 1, false, (float *)(&projection.data));
	CheckGlError();

	setTexture(tempTex->id, 0);
	glUniform1i(renderer->tilemapProgram.u_tilemapTexture, 0);

	setTexture(srcTexture->id, 1);
	glUniform1i(renderer->tilemapProgram.u_tilesetTexture, 1);

	glUniform2i(renderer->tilemapProgram.u_tilemapSize, tilesWide, tilesHigh);
	glUniform2i(renderer->tilemapProgram.u_tilesetSize, srcTexture->width, srcTexture->height);
	glUniform2i(renderer->tilemapProgram.u_resultSize, tilesWide*tileWidth, tilesHigh*tileHeight);
	glUniform2i(renderer->tilemapProgram.u_pixelRatio, tileWidth, tileHeight);

	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	destroyTexture(tempTex);
	CheckGlError();

	glDisableVertexAttribArray(renderer->tilemapProgram.a_position);
	CheckGlError();
}

void drawOneBppBitmap(void *bitmapData, RenderProps *addedProps) {
	RenderProps newProps = *addedProps;
	RenderProps *props = &newProps;

	Texture *tempTex = uploadTexture(bitmapData, props->srcWidth, props->srcHeight);

	setShaderProgram(renderer->oneBppProgram.program);
	CheckGlError();

	glEnableVertexAttribArray(renderer->oneBppProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, 0, 0, props->srcWidth, props->srcHeight);
	glVertexAttribPointer(renderer->oneBppProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	float texX = (float)props->srcX/props->srcWidth;
	float texY = (float)props->srcY/props->srcHeight;
	float texW = (float)props->srcWidth/props->srcWidth;
	float texH = (float)props->srcHeight/props->srcHeight;
	changeArrayBuffer(renderer->tempTexCoords, texX, texY, texX + texW, texY + texH);
	glEnableVertexAttribArray(renderer->oneBppProgram.a_texCoord);
	glVertexAttribPointer(renderer->oneBppProgram.a_texCoord, 2, GL_FLOAT, false, 0, 0);
	CheckGlError();

	Matrix3 matrix;
	matrix.identity();
	matrix.project(renderer->currentViewport.width, renderer->currentViewport.height);
	matrix.translate(props->x, props->y);

	matrix.translate(props->pivotX, props->pivotY);
	matrix.rotate(-props->rotation);
	matrix.scale(props->scaleX, props->scaleY);
	matrix.translate(-props->pivotX, -props->pivotY);

	matrix.translate(props->localX, props->localY);

	glUniformMatrix3fv(renderer->oneBppProgram.u_matrix, 1, false, (float *)matrix.data);

	setTexture(tempTex->id);

	Matrix3 uv;
	uv.identity();
	// uv.scale(1, -1); // Flip UVs to opengl format
	// uv.translate(0, -1); // Flip UVs to opengl format
	glUniformMatrix3fv(renderer->oneBppProgram.u_uv, 1, false, (float *)uv.data);

	glUniform4f(
		renderer->oneBppProgram.u_tint,
		((props->tint >> 16) & 0xff)/255.0,
		((props->tint >> 8) & 0xff)/255.0,
		(props->tint & 0xff)/255.0,
		((props->tint >> 24) & 0xff)/255.0
	);

	glUniform1f(renderer->oneBppProgram.u_alpha, props->alpha);

	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	CheckGlError();

	destroyTexture(tempTex);
	glDisableVertexAttribArray(renderer->oneBppProgram.a_position);
	glDisableVertexAttribArray(renderer->oneBppProgram.a_texCoord);
	CheckGlError();
}


Texture *uploadPngTexturePath(const char *path) {
	void *pngData;
	int pngSize = readFile(path, &pngData);
	Texture *tex = uploadPngTexture(pngData, pngSize);

	if (!tex) {
		printf("Failed to load image %s\n", path);
		exit(1);
	}

	free(pngData);
	return tex;
}

Texture *uploadPngTexture(void *data, int size) {
	int width, height, channels;
	stbi_uc *img = 0;
	img = stbi_load_from_memory((unsigned char *)data, size, &width, &height, &channels, 4);
	if (!img) {
		printf("Failed to parse png bytes");
		return NULL;
	}

	Texture *tex = uploadTexture(img, width, height);
	free(img);

	return tex;
}

Texture *uploadTexture(void *data, int width, int height) {
	GLuint texId;
	glGenTextures(1, &texId);
	setTexture(texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	bool freeData = false;
	if (!data) {
		freeData = true;
		data = malloc(width * height * 4);
		memset(data, 0, width * height * 4);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	CheckGlError();

	if (freeData) free(data);

	Texture *tex = (Texture *)malloc(sizeof(Texture));
	tex->id = texId;
	tex->width = width;
	tex->height = height;

	return tex;
}

void setTextureData(Texture *texture, void *data, int width, int height);
void setTextureData(Texture *texture, void *data, int width, int height) {
	setTexture(texture->id);

	// void glTexImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	CheckGlError();
}

void setShaderProgram(GLuint program) {
	if (renderer->curShaderProgram == program) return;

	glUseProgram(program);
	renderer->curShaderProgram = program;
}

void setArrayBuffer(GLuint buffer) {
	if (renderer->curArrayBuffer == buffer) return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	renderer->curArrayBuffer = buffer;
}

void changeArrayBuffer(GLuint buffer, float x, float y, float width, float height) {
	setArrayBuffer(buffer);

	float bufferData[12] = {
		x, y,
		width, y,
		width, height,
		x, y,
		x, height,
		width, height
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_DYNAMIC_DRAW);
}

GLuint genGlArrayBuffer(float x, float y, float width, float height) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	changeArrayBuffer(buffer, x, y, width, height);
	CheckGlError();
	return buffer;
}

void setTexture(GLuint texture, int slot) {
	if (renderer->currentTexture == texture) return;
	renderer->currentTexture = texture;
	if (slot == 0) glActiveTexture(GL_TEXTURE0);
	if (slot == 1) glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	CheckGlError();
}

void setFramebuffer(GLuint framebuffer) {
	if (renderer->currentFramebuffer == framebuffer) return;

	// I think these are required
	renderer->currentFramebufferTexture = -1;
	renderer->currentTexture = -1;

	renderer->currentFramebuffer = framebuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	CheckGlError();
}

void setFramebufferTexture(GLuint texture) {
	if (renderer->currentFramebufferTexture == texture) return;
	renderer->currentFramebufferTexture = texture;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	CheckGlError();
}

void setViewport(float x, float y, float w, float h) {
	if (renderer->currentViewport.x == x && renderer->currentViewport.y == y && renderer->currentViewport.width == w && renderer->currentViewport.height == h) return;
	renderer->currentViewport.setTo(x, y, w, h);
	glViewport(x, y, w, h);
	CheckGlError();
}

void destroyTexture(Texture *tex) {
	glDeleteTextures(1, &tex->id);
	free(tex);
}

void checkGlError(int lineNum) {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		if (renderer->errorCount < 10) {
			printf("Gl error: %x(%d) at line %d\n", err, err, lineNum);
			renderer->errorCount++;

			if (renderer->errorCount == 10) {
				printf("Max gl errors exceeded, no more will be shown\n");
				break;
			}
		}
	}
}
