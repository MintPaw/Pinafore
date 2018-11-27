#define ANIM_NAME_LIMIT 256
#define ANIM_LIMIT 256

#define POINTS_LIMIT 30
#define COMBAT_TEXT_LIMIT 4096

enum State {
	STATE_NULL=0,
	STATE_BK_IDLE_LEFT,
	STATE_BK_IDLE_RIGHT,
	STATE_FINAL,
};

struct Animation {
	char name[ANIM_NAME_LIMIT];
	int frames[ANIM_LIMIT];
	int framesNum;
	float speed;
	bool loops;
	bool endsAtOne;
};

struct Frame {
	char *name;

	int srcX;
	int srcY;
	int srcWidth;
	int srcHeight;

	int destOffX;
	int destOffY;
	int destWidth;
	int destHeight;

	int absFrame;
};

enum UnitType {
	UNIT_NULL=0,
	UNIT_BLUE_KNIGHT,
};

struct Unit {
	bool exists;

	float x;
	float y;
	UnitType type;
	bool facingLeft;

	int keyToSelect;
};

#define UNIT_LIMIT 1024
struct Game {
	int frameCount;
	float secondCount;
	float lastFrameTime;

	Texture *sprites;

	Animation *spriteAnims;
	Frame *spriteFrames;
	int spriteFramesNum;

	Animation *anims[STATE_FINAL];

	Font arial;
	Texture *fontInterimTexture;

	ImFont *defaultGuiFont;
	ImFont *smallGuiFont;

	Unit units[UNIT_LIMIT];
	int currentLevel;
	Unit *selectedUnit;

	Texture *finalRT;
	Texture *gameRT;
	Texture *tempRT;

	Sound *metro;
};
Game *game = NULL;

void updateGame();
void loadAnimations();
Unit *newUnit();

Frame *getFrame(const char *frameName);
int getAnimFrameAtSecond(Animation *anim, float time);
float getAnimLength(Animation *anim);
int getAnimFrameAtPercent(Animation *anim, float percent);

void setFrameProps(Frame *frame, RenderProps *props);
void drawText(Texture *texture, Font *font, const char *text, int size);

void hotloaderUpdateCallback(const char *path);

void updateGame() {
	if (!game) {
		game = (Game *)malloc(sizeof(Game));
		memset(game, 0, sizeof(Game));
		printf("Game is %0.2f mb btw\n", sizeof(Game) / 1024.0 / 1024.0);

		ImGuiIO &io = ImGui::GetIO();
		char buf[256];

		sprintf(buf, "%sassets/fonts/arial.ttf", platform->filePathPrefix);
		game->defaultGuiFont = io.Fonts->AddFontFromFileTTF(buf, 20);

		sprintf(buf, "%sassets/fonts/arial.ttf", platform->filePathPrefix);
		game->smallGuiFont = io.Fonts->AddFontFromFileTTF(buf, 14);

		game->sprites = uploadPngTexturePath("assets/sprites.png");
		{
			int pixelsUsed = game->sprites->width * game->sprites->height;
			int pixelsAllowed = 4096 * 4096;
			printf("%d/%d (%0.3f) pixels of sprite sheet used\n", pixelsUsed, pixelsAllowed, (float)pixelsUsed/pixelsAllowed*100.0);
		}

		game->gameRT = uploadTexture(NULL, platform->windowWidth, platform->windowHeight);
		game->finalRT = uploadTexture(NULL, platform->windowWidth, platform->windowHeight);
		game->tempRT = uploadTexture(NULL, platform->windowWidth, platform->windowHeight);

		loadAnimations();

		{ /// Setup fonts
			int ttfSize;
			void *ttfData = readFile("assets/fonts/arial.ttf", &ttfSize);
			initFont(ttfData, &game->arial);

			game->fontInterimTexture = uploadTexture(NULL, FONT_WIDTH_LIMIT, FONT_HEIGHT_LIMIT);
		}

		{ /// Setup audio
			game->metro = loadSound("assets/audio/metro.ogg");
			// playSound(game->metro, true);
		}

		{ /// Setup misc
			initEmitters(256);

			watchFile("assets/sprites.png");
			watchFile("assets/sprites.spr");

			hotloader->fileUpdateCallback = hotloaderUpdateCallback;
			reloadAllWatched();
		}
	}

	// #################################################################################################################
	// #################################################################################################################
	// #################################################################################################################
	// #################################################################################################################
	// -------------------------------- UPDATE LOOP START --------------------------------------------------------------
	// #################################################################################################################
	// #################################################################################################################
	// #################################################################################################################
	// #################################################################################################################

	platformStartFrame();
	ImGui::PushFont(game->smallGuiFont);

	NanoTime startTime;
	getNanoTime(&startTime);

	float secondPhase = fabs(sin(game->secondCount*3.14159));
	float elapsed = 1/60.0;

	pushTargetTexture(game->gameRT);
	clearRenderer();

	bool inputLeft = false;
	bool inputRight = false;
	bool inputUp = false;
	bool inputDown = false;
	static bool showDemo = false;
	{ /// Update inputs
		if (keyPressed(KEY_LEFT)) inputLeft = true;
		if (keyPressed(KEY_RIGHT)) inputRight = true;
		if (keyPressed(KEY_UP)) inputUp = true;
		if (keyPressed(KEY_DOWN)) inputDown = true;
		// if (keyJustPressed('A'))  playSound(game->metro);
		if (keyPressed(KEY_CTRL) && keyJustPressed(KEY_BACKTICK)) showDemo = !showDemo;
	}

	if (showDemo) ImGui::ShowDemoWindow(&showDemo);

	// ImGui::Begin("Dev menu", NULL, ImGuiWindowFlags_None);
	if (game->currentLevel == 0) {
		game->currentLevel = 1; 

		Unit *unit1 = newUnit();
		unit1->type = UNIT_BLUE_KNIGHT;
		unit1->x = 100;
		unit1->y = 400;
		unit1->keyToSelect = 'Q';

		Unit *unit2 = newUnit();
		unit2->type = UNIT_BLUE_KNIGHT;
		unit2->x = 200;
		unit2->y = 400;
		unit2->keyToSelect = 'W';
	}

	for (int i = 0; i < UNIT_LIMIT; i++) {
		Unit *unit = &game->units[i];
		if (!unit->exists) continue;

		if (unit->keyToSelect && keyJustPressed(unit->keyToSelect)) {
			game->selectedUnit = unit;
		}

		if (game->selectedUnit == unit) {
			ImGui::SetNextWindowPos(ImVec2(unit->x, unit->y + 100), ImGuiCond_Always, ImVec2(0.5, 0));
			ImGui::Begin("Selected", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("This unit is selected");
			ImGui::End();
		}

		if (unit->type == UNIT_BLUE_KNIGHT) {
			Animation *anim = NULL;
			if (unit->facingLeft) {
				anim = game->anims[STATE_BK_IDLE_LEFT];
			} else {
				anim = game->anims[STATE_BK_IDLE_RIGHT];
			}

			int framesIn = getAnimFrameAtSecond(anim, 0);
			Frame *frame = &game->spriteFrames[anim->frames[framesIn]];

			RenderProps props = newRenderProps();
			setFrameProps(frame, &props);
			props.x += unit->x;
			props.y += unit->y;
			drawTexture(game->sprites, &props);
		}
	}

	{ /// Update emitters
		updateEmitters(1.0);

		for (int i = 0; i < emittersMax; i++) {
			Emitter *emitter = &emitters[i];
			if (!emitter->exists) continue;
			for (int particleI = 0; particleI < emitter->particlesMax; particleI++) {
				Particle *particle = &emitter->particles[particleI]; 
				if (!particle->exists) continue;

				if (emitter->frameName[0] == '\0') {
					drawCircle(particle->pos.x, particle->pos.y, 5*particle->scale, particle->colour);
				} else {
					RenderProps particleProps = newRenderProps();
					particleProps.scaleX = particleProps.scaleY = particle->scale;
					particleProps.tint = particle->colour;
					particleProps.alpha = particle->alpha;
					// drawFrame(getFrame(emitter->frameName), particle->pos.x, particle->pos.y, &particleProps);
				}
			}
		}
	}

	popTargetTexture();

	pushTargetTexture(game->finalRT);
	clearRenderer(1, 1, 1, 1);

	RenderProps gameProps = newRenderProps();
	drawTexture(game->gameRT, &gameProps);

	popTargetTexture();

#ifdef MULTISAMPLING
	blitFramebuffer(game->finalRT);
#else
	RenderProps finalProps = newRenderProps();
	drawTexture(game->finalRT, &finalProps);
#endif

	updateAudio();
	game->frameCount++;
	game->secondCount += elapsed;

	ImGui::PopFont();
	swapBuffers();

	NanoTime endTime;
	getNanoTime(&endTime);
	game->lastFrameTime = getMsPassed(&startTime, &endTime);
	// if (game->frameCount % 60 == 0) printf("ft: %f\n", game->lastFrameTime);
}

Unit *newUnit() {
	for (int i = 0; i < UNIT_LIMIT; i++) {
		Unit *unit = &game->units[i];
		if (!unit->exists) {
			memset(unit, 0, sizeof(Unit));
			unit->exists = true;
			return unit;
		}
	}

	//TODO: Log no more units
	return NULL;
}

void loadAnimations() {
	{ /// Setup frames
		int sprSize;
		char *sprData = (char *)readFile("assets/sprites.spr", &sprSize);

		int newLineCount = 1;
		for (int i = 0; i < sprSize; i++)
			if (sprData[i] == '\n') newLineCount++;

		int totalFrames = newLineCount/3;

		if (game->spriteFrames) free(game->spriteFrames);
		game->spriteFrames = (Frame *)malloc(totalFrames*sizeof(Frame));
		game->spriteFramesNum = totalFrames;

		char *lineStart = sprData;

		bool moreLines = true;
		for (int i = 0; moreLines; i++) {
			char *lineEnd = strchr(lineStart, '\n');
			if (!lineEnd) {
				moreLines = false;
				lineEnd = &sprData[strlen(sprData)-1];
			}

			int lineLen = lineEnd-lineStart;
			char line[256];
			if (lineLen <= 0) break;
			strncpy(line, lineStart, lineLen);
			line[lineLen] = '\0';

			Frame *frame = &game->spriteFrames[i/3];
			frame->absFrame = i/3;
			if (i % 3 == 0) {
				frame->name = stringClone(line);
			} else if (i % 3 == 1) {
				sscanf(line, "%d %d %d %d", &frame->srcX, &frame->srcY, &frame->srcWidth, &frame->srcHeight);
			} else if (i % 3 == 2) {
				sscanf(line, "%d %d %d %d", &frame->destOffX, &frame->destOffY, &frame->destWidth, &frame->destHeight);
			}

			lineStart = lineEnd + 1;
		}

		free(sprData);

		// for (int i = 0; i < game->spriteFramesNum; i++) {
		// 	Frame *frame = &game->spriteFrames[i];
		// 	printf("Frame %s: %d %d %d %d, %d %d %d %d\n", frame->name, frame->srcX, frame->srcY, frame->srcWidth, frame->srcHeight, frame->destOffX, frame->destOffY, frame->destWidth, frame->destHeight);
		// }
	}

	{ /// Setup animations
		char tempAnimNames[ANIM_NAME_LIMIT][ANIM_LIMIT];
		int tempAnimNamesNum = 0;

		for (int i = 0; i < game->spriteFramesNum; i++) {
			Frame *frame = &game->spriteFrames[i];
			char *nameEnd = NULL;
			nameEnd = strrchr(frame->name, '_');
			if (!nameEnd) nameEnd = frame->name + strlen(frame->name);
			int nameLen = nameEnd - frame->name;

			char curName[ANIM_NAME_LIMIT];
			strncpy(curName, frame->name, nameLen);
			curName[nameLen] = '\0';

			bool newAnim = true;
			for (int tempNamesI = 0; tempNamesI < tempAnimNamesNum; tempNamesI++) {
				if (streq(tempAnimNames[tempNamesI], curName)) newAnim = false;
			}

			if (newAnim) strcpy(tempAnimNames[tempAnimNamesNum++], curName);
		}

		if (game->spriteAnims) free(game->spriteAnims);
		game->spriteAnims = (Animation *)malloc(tempAnimNamesNum*sizeof(Animation));
		memset(game->spriteAnims, 0, tempAnimNamesNum*sizeof(Animation));

		for (int i = 0; i < tempAnimNamesNum; i++) {
			Animation *anim = &game->spriteAnims[i];
			anim->speed = 1;
			strcpy(anim->name, tempAnimNames[i]);

			for (int frameI = 0; frameI < game->spriteFramesNum; frameI++) {
				Frame *frame = &game->spriteFrames[frameI];
				char *nameEnd = strrchr(frame->name, '_');
				if (!nameEnd) nameEnd = frame->name + strlen(frame->name);
				int nameLen = nameEnd - frame->name;

				char curName[ANIM_NAME_LIMIT];
				strncpy(curName, frame->name, nameLen);
				curName[nameLen] = '\0';

				if (streq(curName, anim->name)) anim->frames[anim->framesNum++] = frameI;
			}
		}

		for (int i = 0; i < tempAnimNamesNum; i++) {
			Animation *anim = &game->spriteAnims[i];

			if (streq(anim->name, "blueKnight/BKIdleLL1")) {
				// anim->speed = 0.2;
				anim->loops = true;
				game->anims[STATE_BK_IDLE_LEFT] = anim;
			} else if (streq(anim->name, "blueKnight/BKIdleRL1")) {
				// anim->speed = 0.2;
				anim->loops = true;
				game->anims[STATE_BK_IDLE_RIGHT] = anim;
			}
		}

		int animCount = ArrayLength(game->anims);
		for (int i = 1; i < animCount; i++) {
			if (!game->anims[i]) {
				printf("Anim number %d missing\n", i);
				Assert(0);
			}
		}
	}
}

Frame *getFrame(const char *frameName) {
	for (int i = 0; i < game->spriteFramesNum; i++) {
		Frame *frame = &game->spriteFrames[i];
		if (streq(frame->name, frameName)) return frame;
	}

	return NULL;
}

void setFrameProps(Frame *frame, RenderProps *props) {
	props->srcX = frame->srcX;
	props->srcY = frame->srcY;
	props->srcWidth = frame->srcWidth;
	props->srcHeight = frame->srcHeight;
	props->localX = frame->destOffX;
	props->localY = frame->destOffY;
	props->pivotX = frame->destWidth/2;
	props->pivotY = frame->destHeight/2;
	props->x += -frame->destWidth/2;
	props->y += -frame->destHeight/2;
}

void drawText(Texture *texture, Font *font, const char *text, int size) {
	generateText(font, text, size);
	setTextureData(game->fontInterimTexture, currentTextBitmap4bpp, FONT_WIDTH_LIMIT, FONT_HEIGHT_LIMIT);
	pushTargetTexture(texture);
	clearRenderer();
	RenderProps textProps = newRenderProps();
	textProps.scrollFactor = 0;
	drawTexture(game->fontInterimTexture, &textProps);
	popTargetTexture();
}

int getAnimFrameAtPercent(Animation *anim, float percent) {
	int framesIn = (anim->framesNum-1) * percent;

	if (anim->loops) {
		framesIn = framesIn % anim->framesNum;
	} else {
		if (framesIn > anim->framesNum - 1) framesIn = anim->framesNum - 1;
	}

	if (anim->endsAtOne && framesIn == anim->framesNum - 1) framesIn = 0;

	return framesIn;
}

int getAnimFrameAtSecond(Animation *anim, float time) {
	float frameRate = 10.0 * anim->speed;
	int framesIn = frameRate * time;

	if (anim->loops) {
		framesIn = framesIn % anim->framesNum;
	} else {
		if (framesIn > anim->framesNum - 1) framesIn = anim->framesNum - 1;
	}

	if (anim->endsAtOne && framesIn == anim->framesNum - 1) framesIn = 0;

	return framesIn;
}

float getAnimLength(Animation *anim) {
	return anim->framesNum / (10.0 * anim->speed);
}

void hotloaderUpdateCallback(const char *path) {
#if 0 // Add a catalog before uncommmenting
	printf("Updating file %s\n", path);
	Catalog *catalogs[] = {
	};
	int catalogsNum = ArrayLength(catalogs);

	for (int i = 0; i < catalogsNum; i++) {
		Catalog *cat = catalogs[i];
		if (streq(cat->path, path)) {
			pullCatalog(cat);

			printf("Catalog %s has been reloaded\n", cat->path);
			return;
		}
	}

	if (streq(path, "assets/sprites.png")) {
		destroyTexture(game->sprites);
		game->sprites = uploadPngTexturePath("assets/sprites.png");
	}

	if (streq(path, "assets/sprites.spr")) {
		loadAnimations();
	}
#endif
}
