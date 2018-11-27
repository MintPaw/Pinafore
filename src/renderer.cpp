#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TARGET_TEXTURE_LIMIT 16
// #define MULTISAMPLING

#define GetAttrib(programName, attribName) programName.attribName = glGetAttribLocation(programName.program, #attribName) 
#define GetUniform(programName, uniformName) programName.uniformName = glGetUniformLocation(programName.program, #uniformName) 
struct RenderProps;
struct Texture;

void initRenderer(int screenWidth, int screenHeight);
GLuint buildShader(const char *vertPath, const char *fragPath);

void setShaderProgram(GLuint program);
void changeArrayBuffer(GLuint glBuffer, float x, float y, float width, float height);
void changeArrayBuffer(GLuint buffer, float *verts, int vertsNum);
void setViewport(float x, float y, float w, float h);
void destroyTexture(Texture *tex);

Texture *uploadPngTexturePath(const char *path);
Texture *uploadPngTexture(void *data, int size);
Texture *uploadTexture(void *data, int width, int height);
void setTextureData(Texture *texture, void *data, int width, int height);

void clearRenderer(float r=0, float g=0, float b=0, float a=0);
RenderProps newRenderProps();

void pushTargetTexture(Texture *texture);
void popTargetTexture();
void setTargetTexture(Texture *texture);
void resetTargetTexture();

void drawCircle(float x, float y, float radius, int colour);
void drawRect(float x, float y, float width, float height, int colour);
void drawTiles(Texture *srcTexture, int destWidth, int destHeight, int tileWidth, int tileHeight, int tilesWide, int tilesHigh, int *tiles);
void drawTexture(Texture *texture, RenderProps *addedProps);
void draw3dTriangle(Texture *texture=NULL);
void blitFramebuffer();

void drawInverted(Texture *texture);
void drawOutlined(Texture *texture, int outlineColour, float outlineSize=1);
void drawGaussianBlurredX(Texture *texture);
void drawGaussianBlurredY(Texture *texture);

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
	bool smooth;

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
	GLuint u_texture;
};

struct Tri3dProgram {
	GLuint program;
	GLuint a_position;
	GLuint a_texCoord;
	GLuint u_texture;
};

struct EffectProgram {
	GLuint program;
	GLuint a_position;
	GLuint a_texCoord;
	GLuint u_matrix;
	GLuint u_texture;
	GLuint u_invert;
};

struct OutlineProgram {
	GLuint program;
	GLuint a_position;
	GLuint a_texCoord;
	GLuint u_matrix;
	GLuint u_texture;
	GLuint u_viewportInverse;
	GLuint u_outlineSize;
	GLuint u_outlineColour;
};

struct GaussianBlurXProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_imageSize;
	GLuint u_texture;
};

struct GaussianBlurYProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_imageSize;
	GLuint u_texture;
};

struct Renderer {
	int errorCount;
	int screenWidth;
	int screenHeight;
	bool enabled;
	Rect camera;

	RectProgram rectProgram;
	CircleProgram circleProgram;
	DefaultProgram defaultProgram;
	TilemapProgram tilemapProgram;
	Tri3dProgram tri3dProgram;
	EffectProgram effectProgram;
	OutlineProgram outlineProgram;
	GaussianBlurXProgram gaussianBlurXProgram;
	GaussianBlurYProgram gaussianBlurYProgram;

	Texture *currentTargetTexture;
	Texture *targetTextureStack[TARGET_TEXTURE_LIMIT];
	int targetTextureStackNum;

	GLuint tempVerts;
	GLuint tempTexCoords;
	GLuint textureFramebuffer;

	GLuint curShaderProgram;
	GLuint curArrayBuffer;
	GLuint currentFramebuffer;
	Rect currentViewport;

	GLuint texture2dType;
};

Renderer *renderer;

/// End header

void initRenderer(int screenWidth, int screenHeight) {
	printf("Initing renderer\n");

#ifdef __EMSCRIPTEN__
	printf("GL error check disabled\n");
#endif

	renderer = (Renderer *)malloc(sizeof(Renderer));
	memset(renderer, 0, sizeof(Renderer));
	renderer->enabled = true;
	renderer->screenWidth = screenWidth;
	renderer->screenHeight = screenHeight;
#ifdef MULTISAMPLING
	renderer->texture2dType = GL_TEXTURE_2D_MULTISAMPLE;
#else
	renderer->texture2dType = GL_TEXTURE_2D;
#endif
	stbi_set_flip_vertically_on_load(true);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	if (renderer->texture2dType == GL_TEXTURE_2D_MULTISAMPLE) {
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		glEnable(GL_SAMPLE_ALPHA_TO_ONE);
		glEnable(GL_MULTISAMPLE);
	}
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
	GetUniform(renderer->defaultProgram, u_texture);

	renderer->tilemapProgram.program = buildShader("assets/shaders/tilemap.vert", "assets/shaders/tilemap.frag");
	GetAttrib(renderer->tilemapProgram, a_position);
	GetUniform(renderer->tilemapProgram, u_matrix);
	GetUniform(renderer->tilemapProgram, u_tilesetTexture);
	GetUniform(renderer->tilemapProgram, u_tilemapTexture);
	GetUniform(renderer->tilemapProgram, u_tilesetSize);
	GetUniform(renderer->tilemapProgram, u_tilemapSize);
	GetUniform(renderer->tilemapProgram, u_resultSize);
	GetUniform(renderer->tilemapProgram, u_pixelRatio);

	renderer->tri3dProgram.program = buildShader("assets/shaders/tri3d.vert", "assets/shaders/tri3d.frag");
	GetAttrib(renderer->tri3dProgram, a_position);
	GetAttrib(renderer->tri3dProgram, a_texCoord);
	GetUniform(renderer->tri3dProgram, u_texture);

	renderer->effectProgram.program = buildShader("assets/shaders/effects.vert", "assets/shaders/effects.frag");
	GetAttrib(renderer->effectProgram, a_position);
	GetAttrib(renderer->effectProgram, a_texCoord);
	GetUniform(renderer->effectProgram, u_matrix);
	GetUniform(renderer->effectProgram, u_texture);
	GetUniform(renderer->effectProgram, u_invert);

	renderer->outlineProgram.program = buildShader("assets/shaders/outline.vert", "assets/shaders/outline.frag");
	GetAttrib(renderer->outlineProgram, a_position);
	GetAttrib(renderer->outlineProgram, a_texCoord);
	GetUniform(renderer->outlineProgram, u_matrix);
	GetUniform(renderer->outlineProgram, u_texture);
	GetUniform(renderer->outlineProgram, u_viewportInverse);
	GetUniform(renderer->outlineProgram, u_outlineSize);
	GetUniform(renderer->outlineProgram, u_outlineColour);

	renderer->gaussianBlurXProgram.program = buildShader("assets/shaders/gaussianBlurX.vert", "assets/shaders/gaussianBlur.frag");
	GetAttrib(renderer->gaussianBlurXProgram, a_position);
	GetUniform(renderer->gaussianBlurXProgram, u_matrix);
	GetUniform(renderer->gaussianBlurXProgram, u_texture);
	GetUniform(renderer->gaussianBlurXProgram, u_imageSize);

	renderer->gaussianBlurYProgram.program = buildShader("assets/shaders/gaussianBlurY.vert", "assets/shaders/gaussianBlur.frag");
	GetAttrib(renderer->gaussianBlurYProgram, a_position);
	GetUniform(renderer->gaussianBlurYProgram, u_matrix);
	GetUniform(renderer->gaussianBlurYProgram, u_texture);
	GetUniform(renderer->gaussianBlurYProgram, u_imageSize);

	CheckGlError();

	glGenBuffers(1, &renderer->tempVerts);
	glGenBuffers(1, &renderer->tempTexCoords);
	glGenFramebuffers(1, &renderer->textureFramebuffer);
	CheckGlError();

	pushTargetTexture(0);
}

GLuint buildShader(const char *vertPath, const char *fragPath) {
	char *vertSrc = (char *)readFile(vertPath);
	char *fragSrc = (char *)readFile(fragPath);

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
		printf("%s result is: %d\nError(%d):\n%s\n", vertPath, vertReturn, errLogNum, errLog);
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
		printf("%s result is: %d\nError:\n%s\n", fragPath, fragReturn, errLog);
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
	if (!renderer->enabled) return;

	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

RenderProps newRenderProps() {
	RenderProps props = {};
	props.scaleX = 1;
	props.scaleY = 1;
	props.alpha = 1;
	props.scrollFactor = 1;
	props.smooth = true;
	return props;
}

void pushTargetTexture(Texture *texture) {
	if (renderer->targetTextureStackNum >= TARGET_TEXTURE_LIMIT-1) {
		printf("Target texture overflow");
		Assert(0);
	}

	renderer->targetTextureStack[renderer->targetTextureStackNum++] = renderer->currentTargetTexture;
	renderer->currentTargetTexture = texture;

	setTargetTexture(renderer->currentTargetTexture);
};

void popTargetTexture() {
	if (renderer->targetTextureStackNum <= 0) {
		renderer->currentTargetTexture = NULL;
		setTargetTexture(NULL);
		return;
	}

	renderer->currentTargetTexture = renderer->targetTextureStack[renderer->targetTextureStackNum-1];
	renderer->targetTextureStackNum--;
	setTargetTexture(renderer->currentTargetTexture);
}

void resetTargetTexture() {
	setTargetTexture(NULL);
	renderer->targetTextureStackNum = 0;
}

void setTargetTexture(Texture *texture) {
	if (!renderer->enabled) return;

	if (texture == NULL) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		setViewport(0, 0, renderer->screenWidth, renderer->screenHeight);
	} else {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->textureFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderer->texture2dType, texture->id, 0);
		setViewport(0, 0, texture->width, texture->height);
	}

	CheckGlError();
}

void drawRect(float x, float y, float width, float height, int colour) {
	if (!renderer->enabled) return;

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

void drawCircle(float x, float y, float radius, int colour) {
	if (!renderer->enabled) return;

	setShaderProgram(renderer->circleProgram.program);
	CheckGlError();

	x -= radius;
	y -= radius;

	x -= renderer->camera.x;
	y -= renderer->camera.y;

	radius *= 2;

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
	if (!renderer->enabled) return;

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

	Vec2 camOff = {renderer->camera.x*props->scrollFactor, renderer->camera.y*props->scrollFactor};

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
	glUniform1i(renderer->defaultProgram.u_texture, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texture->id);

	if (renderer->texture2dType == GL_TEXTURE_2D) {
		if (props->smooth) {
			glTexParameteri(renderer->texture2dType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(renderer->texture2dType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(renderer->texture2dType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(renderer->texture2dType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

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
	if (!renderer->enabled) return;

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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, tempTex->id);
	glUniform1i(renderer->tilemapProgram.u_tilemapTexture, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(renderer->texture2dType, srcTexture->id);
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

void draw3dTriangle(Texture *texture) {
	if (!renderer->enabled) return;

	setShaderProgram(renderer->tri3dProgram.program);

	GLfloat data[15] = {
		(float)-0.8, (float)-0.8, (float)0.0, (float)0.0, (float)0.0,
		 (float)0.8, (float)-0.8, (float)0.0, (float)1.0, (float)0.0,
		 (float)0.0,  (float)0.8, (float)0.0, (float)0.5, (float)1.0
	};

	glEnableVertexAttribArray(renderer->tri3dProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, data, 15);
	glVertexAttribPointer(renderer->tri3dProgram.a_position, 3, GL_FLOAT, false, sizeof(GLfloat)*5, NULL);
	CheckGlError();

	glEnableVertexAttribArray(renderer->tri3dProgram.a_texCoord);
	glVertexAttribPointer(renderer->tri3dProgram.a_texCoord, 2, GL_FLOAT, false, sizeof(GLfloat)*5, (void *)(sizeof(GLfloat)*3));
	CheckGlError();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texture->id);
	glUniform1i(renderer->tri3dProgram.u_texture, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	CheckGlError();

	glDisableVertexAttribArray(renderer->tri3dProgram.a_position);
	glDisableVertexAttribArray(renderer->tri3dProgram.a_texCoord);
	CheckGlError();
}

void blitFramebuffer(Texture *tex) {
	if (!renderer->enabled) return;

	resetTargetTexture();
	CheckGlError();
	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->textureFramebuffer);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderer->texture2dType, tex->id, 0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(
		0, 0, renderer->screenWidth, renderer->screenHeight,
		0, 0, renderer->screenWidth, renderer->screenHeight,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST
	);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	CheckGlError();
}

void drawInverted(Texture *texture) {
	if (!renderer->enabled) return;

	setShaderProgram(renderer->effectProgram.program);
	CheckGlError();

	glEnableVertexAttribArray(renderer->effectProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, 0, 0, texture->width, texture->height);
	glVertexAttribPointer(renderer->effectProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	glEnableVertexAttribArray(renderer->effectProgram.a_texCoord);
	changeArrayBuffer(renderer->tempTexCoords, 0, 0, 1, 1);
	glVertexAttribPointer(renderer->effectProgram.a_texCoord, 2, GL_FLOAT, false, 0, 0);
	CheckGlError();

	Matrix3 matrix;
	matrix.identity();
	matrix.project(renderer->currentViewport.width, renderer->currentViewport.height);

	glUniformMatrix3fv(renderer->effectProgram.u_matrix, 1, false, (float *)matrix.data);

	glUniform1i(renderer->effectProgram.u_invert, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texture->id);

	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	CheckGlError();

	glDisableVertexAttribArray(renderer->effectProgram.a_position);
	glDisableVertexAttribArray(renderer->effectProgram.a_texCoord);
	CheckGlError();
};

void drawOutlined(Texture *texture, int outlineColour, float outlineSize) {
	if (!renderer->enabled) return;

	setShaderProgram(renderer->outlineProgram.program);
	CheckGlError();

	glEnableVertexAttribArray(renderer->outlineProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, 0, 0, texture->width, texture->height);
	glVertexAttribPointer(renderer->outlineProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	glEnableVertexAttribArray(renderer->outlineProgram.a_texCoord);
	changeArrayBuffer(renderer->tempTexCoords, 0, 0, 1, 1);
	glVertexAttribPointer(renderer->outlineProgram.a_texCoord, 2, GL_FLOAT, false, 0, 0);
	CheckGlError();

	Matrix3 matrix;
	matrix.identity();
	matrix.project(renderer->currentViewport.width, renderer->currentViewport.height);

	glUniformMatrix3fv(renderer->outlineProgram.u_matrix, 1, false, (float *)matrix.data);

	glUniform2f(renderer->outlineProgram.u_viewportInverse, 1.0/texture->width, 1.0/texture->height);
	glUniform1f(renderer->outlineProgram.u_outlineSize, outlineSize);

	glUniform4f(
		renderer->outlineProgram.u_outlineColour,
		((outlineColour >> 16) & 0xff)/255.0,
		((outlineColour >> 8) & 0xff)/255.0,
		(outlineColour & 0xff)/255.0,
		((outlineColour >> 24) & 0xff)/255.0
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texture->id);

	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	CheckGlError();

	glDisableVertexAttribArray(renderer->outlineProgram.a_position);
	glDisableVertexAttribArray(renderer->outlineProgram.a_texCoord);
	CheckGlError();
};

void drawGaussianBlurredX(Texture *texture) {
	if (!renderer->enabled) return;

	setShaderProgram(renderer->gaussianBlurXProgram.program);
	CheckGlError();

	glEnableVertexAttribArray(renderer->gaussianBlurXProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, 0, 0, texture->width, texture->height);
	glVertexAttribPointer(renderer->gaussianBlurXProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	Matrix3 matrix;
	matrix.identity();
	matrix.project(renderer->currentViewport.width, renderer->currentViewport.height);

	glUniformMatrix3fv(renderer->gaussianBlurXProgram.u_matrix, 1, false, (float *)matrix.data);

	glUniform2f(renderer->gaussianBlurXProgram.u_imageSize, texture->width, texture->height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texture->id);

	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	CheckGlError();

	glDisableVertexAttribArray(renderer->gaussianBlurXProgram.a_position);
	CheckGlError();
};

void drawGaussianBlurredY(Texture *texture) {
	if (!renderer->enabled) return;

	setShaderProgram(renderer->gaussianBlurYProgram.program);
	CheckGlError();

	glEnableVertexAttribArray(renderer->gaussianBlurYProgram.a_position);
	changeArrayBuffer(renderer->tempVerts, 0, 0, texture->width, texture->height);
	glVertexAttribPointer(renderer->gaussianBlurYProgram.a_position, 2, GL_FLOAT, false, 0, NULL);
	CheckGlError();

	Matrix3 matrix;
	matrix.identity();
	matrix.project(renderer->currentViewport.width, renderer->currentViewport.height);

	glUniformMatrix3fv(renderer->gaussianBlurYProgram.u_matrix, 1, false, (float *)matrix.data);

	glUniform2f(renderer->gaussianBlurYProgram.u_imageSize, texture->width, texture->height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texture->id);

	CheckGlError();

	glDrawArrays(GL_TRIANGLES, 0, 2*3);
	CheckGlError();

	glDisableVertexAttribArray(renderer->gaussianBlurYProgram.a_position);
	CheckGlError();
};

Texture *uploadPngTexturePath(const char *path) {
	int pngSize;
	void *pngData = readFile(path, &pngSize);
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texId);

	if (renderer->texture2dType == GL_TEXTURE_2D_MULTISAMPLE) {
		glTexImage2DMultisample(renderer->texture2dType, 4, GL_RGBA, width, height, GL_FALSE);
	} else {
		if (renderer->texture2dType == GL_TEXTURE_2D) {
			glTexParameteri(renderer->texture2dType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(renderer->texture2dType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(renderer->texture2dType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	CheckGlError();

	Texture *tex = (Texture *)malloc(sizeof(Texture));
	tex->id = texId;
	tex->width = width;
	tex->height = height;
	if (data) setTextureData(tex, data, width, height);

	return tex;
}

void setTextureData(Texture *texture, void *data, int width, int height) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(renderer->texture2dType, texture->id);

	// void glTexImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
	glTexSubImage2D(renderer->texture2dType, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	CheckGlError();
}

void setShaderProgram(GLuint program) {
	if (renderer->curShaderProgram == program) return;

	glUseProgram(program);
	renderer->curShaderProgram = program;
}

void changeArrayBuffer(GLuint buffer, float x, float y, float width, float height) {
	float bufferData[12] = {
		x, y,
		width, y,
		width, height,
		x, y,
		x, height,
		width, height
	};

	changeArrayBuffer(buffer, bufferData, 12);
}

void changeArrayBuffer(GLuint buffer, float *verts, int vertsNum) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * vertsNum, verts, GL_DYNAMIC_DRAW);
}

void setViewport(float x, float y, float w, float h) {
	renderer->currentViewport.setTo(x, y, w, h);
	glViewport(x, y, w, h);
	CheckGlError();
}

void destroyTexture(Texture *tex) {
	glDeleteTextures(1, &tex->id);
	free(tex);
}

void checkGlError(int lineNum) {
	if (!renderer->enabled) return;
#ifndef __EMSCRIPTEN__
	for (;;) {
		GLenum err = glGetError();
		if (err == GL_NO_ERROR) break;

		if (renderer->errorCount < 10) {
			printf("Gl error: %x(%d) at line %d\n", err, err, lineNum);
			renderer->errorCount++;

			if (renderer->errorCount == 10) {
				printf("Max gl errors exceeded, no more will be shown\n");
				break;
			}
		}
	}
#endif
}
