#define UNIT_LIMIT 256
#define ANIM_NAME_LIMIT 256
#define ANIM_FRAMES_LIMIT 256

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

struct Animation {
	char name[ANIM_NAME_LIMIT];
	int frames[ANIM_FRAMES_LIMIT];
	int framesNum;
	float speed;
	bool loops;
};

enum UnitType {
	UNIT_NULL=0,
	UNIT_BK,
	UNIT_RM,
};

enum State {
	STATE_NULL=0,

	STATE_BK_IDLE_LEFT,
	STATE_BK_IDLE_RIGHT,
	STATE_BK_WALK_LEFT,
	STATE_BK_WALK_RIGHT,
	STATE_BK_ATTACK_LEFT,
	STATE_BK_ATTACK_RIGHT,
	STATE_BK_TRANS_LEFT_TO_RIGHT,
	STATE_BK_TRANS_RIGHT_TO_LEFT,

	STATE_RM_IDLE_LEFT,
	STATE_RM_IDLE_RIGHT,
	STATE_RM_WALK_LEFT,
	STATE_RM_WALK_RIGHT,
	STATE_RM_ATTACK_LEFT,
	STATE_RM_ATTACK_RIGHT,
	STATE_RM_TRANS_LEFT_TO_RIGHT,
	STATE_RM_TRANS_RIGHT_TO_LEFT,

	STATE_FINAL,
};

struct Unit {
	bool exists;
	bool isPlayer;

	UnitType type;
	State prevState;
	State state;

	Animation *currentAnim;
	int frameAnimStarted;
	bool facingRight;

	State idleLeft;
	State idleRight;
	State walkLeft;
	State walkRight;
	State transLeftToRight;
	State transRightToLeft;

	float x;
	float y;
	float collWidth;
	float collHeight;

	Point moveAccel;

	Point velo;
	Point accel;
	Point drag;

	int memory[8];
};

struct Game {
	int frameCount;
	float timeScale;

	Unit units[UNIT_LIMIT];
	Unit *player;

	Frame *spriteFrames;
	int spriteFramesNum;

	Texture *sprites;

	Animation *anims[STATE_FINAL];

	tinytiled_map_t *tiledMap; 
	Texture *mapTexture;
	Texture *tilesetTexture;
	int tileSize;
	int tilesWide;
	int tilesHigh;
	int *collTiles;
};

Game *game = NULL;

void updateGame();

Unit *newUnit(UnitType type);
void drawFrame(Frame *frame, float xpos, float ypos, bool flipX);

int getRealTimeFramesInState(Unit *unit, int atFrame=-1);
int getAnimFramesInState(Unit *unit, int atFrame=-1);
int getCurrentAnimFrame(Unit *unit, int atFrame=-1);

int qsortActiveUnits(const void *a, const void *b);

void updateGame() {
	if (!game) {
		game = (Game *)malloc(sizeof(Game));
		memset(game, 0, sizeof(Game));
		printf("Game is %0.2f mb btw\n", sizeof(Game) / 1024.0 / 1024.0);

		game->sprites = uploadPngTexturePath("assets/sprites.png");
		game->timeScale = 1;

		{/// Setup frames
			char *sprData;
			long sprSize = readFile("assets/sprites.spr", &(void *)sprData);

			int newLineCount = 1;
			for (int i = 0; i < sprSize; i++)
				if (sprData[i] == '\n') newLineCount++;

			int totalFrames = newLineCount/3;

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
		}

		{ /// Setup animations
			char tempAnimNames[ANIM_NAME_LIMIT][ANIM_FRAMES_LIMIT];
			int tempAnimNamesNum = 0;

			for (int i = 0; i < game->spriteFramesNum; i++) {
				Frame *frame = &game->spriteFrames[i];

				char *nameEnd = NULL;
				for (int endNum = 0; endNum < 9; endNum++) {
					nameEnd = strrchr(frame->name, '0'+endNum);
					if (nameEnd) break;
				}
				if (!nameEnd) {
					printf("No name end found on name %s\n", frame->name);
					Assert(0);
				}
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

			Animation *spriteAnims = (Animation *)malloc(tempAnimNamesNum*sizeof(Animation));
			memset(spriteAnims, 0, tempAnimNamesNum*sizeof(Animation));

			for (int i = 0; i < tempAnimNamesNum; i++) {
				Animation *anim = &spriteAnims[i];
				anim->speed = 1;
				strcpy(anim->name, tempAnimNames[i]);

				for (int frameI = 0; frameI < game->spriteFramesNum; frameI++) {
					Frame *frame = &game->spriteFrames[frameI];

					char *nameEnd = NULL;
					for (int endNum = 0; endNum < 9; endNum++) {
						nameEnd = strrchr(frame->name, '0'+endNum);
						if (nameEnd) break;
					}

					int nameLen = nameEnd - frame->name;

					char curName[ANIM_NAME_LIMIT];
					strncpy(curName, frame->name, nameLen);
					curName[nameLen] = '\0';

					if (streq(curName, anim->name)) anim->frames[anim->framesNum++] = frameI;
				}
			}

			for (int i = 0; i < tempAnimNamesNum; i++) {
				Animation *anim = &spriteAnims[i];

				if (streq(anim->name, "blueKnight/BKIdleLL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_BK_IDLE_LEFT] = anim;
				} else if (streq(anim->name, "blueKnight/BKIdleRL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_BK_IDLE_RIGHT] = anim;
				} else if (streq(anim->name, "blueKnight/BKWalkLL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_BK_WALK_LEFT] = anim;
				} else if (streq(anim->name, "blueKnight/BKWalkRL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_BK_WALK_RIGHT] = anim;
				} else if (streq(anim->name, "blueKnight/BKAtkLL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_BK_ATTACK_LEFT] = anim;
				} else if (streq(anim->name, "blueKnight/BKAtkRL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_BK_ATTACK_RIGHT] = anim;
				} else if (streq(anim->name, "blueKnight/BKTransLL")) {
					anim->speed = 0.2;
					game->anims[STATE_BK_TRANS_LEFT_TO_RIGHT] = anim;
				} else if (streq(anim->name, "blueKnight/BKTransRR")) {
					anim->speed = 0.2;
					game->anims[STATE_BK_TRANS_RIGHT_TO_LEFT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMIdleLL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_RM_IDLE_LEFT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMIdleRL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_RM_IDLE_RIGHT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMWalkLL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_RM_WALK_LEFT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMWalkRL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_RM_WALK_RIGHT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMAtkLL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_RM_ATTACK_LEFT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMAtkRL")) {
					anim->loops = true;
					anim->speed = 0.2;
					game->anims[STATE_RM_ATTACK_RIGHT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMTransLL")) {
					anim->speed = 0.2;
					game->anims[STATE_RM_TRANS_LEFT_TO_RIGHT] = anim;
				} else if (streq(anim->name, "redMinotaur/RMTransRR")) {
					anim->speed = 0.2;
					game->anims[STATE_RM_TRANS_RIGHT_TO_LEFT] = anim;
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

		{ /// Setup map
			// game->tilesetTexture = uploadPngTexturePath("assets/tileset.png");

			// void *mapData;
			// long mapSize = readFile("assets/maps/testMap.json", &mapData);

			// game->tiledMap = tinytiled_load_map_from_memory(mapData, mapSize, 0);
			// game->tileSize = game->tiledMap->tilewidth;
			// game->tilesWide = game->tiledMap->width;
			// game->tilesHigh = game->tiledMap->height;
			// game->mapTexture = uploadTexture(NULL, game->tilesWide * game->tileSize, game->tilesHigh * game->tileSize);

			// tinytiled_layer_t *layer = game->tiledMap->layers;
			// while (layer) {
			// 	if (streq(layer->name.ptr, "visual")) {
			// 		setTargetTexture(game->mapTexture);
			// 		drawTiles(game->tilesetTexture, game->mapTexture->width, game->mapTexture->height, game->tileSize, game->tileSize, game->tiledMap->width, game->tiledMap->height, layer->data);
			// 		setTargetTexture(0);
			// 	}

			// 	if (streq(layer->name.ptr, "coll")) {
			// 		game->collTiles = layer->data;
			// 	}

			// 	layer = layer->next;
			// }
		}

		/// Setup player
		game->player = newUnit(UNIT_BK);
		game->player->isPlayer = true;
		game->player->x = platform->windowWidth/2;
		game->player->y = platform->windowHeight/2;

		/// Setup enemies
		Unit *enemy1 = newUnit(UNIT_RM);
		enemy1->x = 200;
		enemy1->y = 200;

		Unit *enemy2 = newUnit(UNIT_RM);
		enemy2->x = platform->windowWidth - 200;
		enemy2->y = platform->windowHeight - 200;
	}

	clearRenderer();

	// RenderProps mapProps = newRenderProps();
	// drawTexture(game->mapTexture, &mapProps);

	bool inputLeft = false;
	bool inputRight = false;
	bool inputUp = false;
	bool inputDown = false;
	bool inputJump = false;
	bool inputAttack = false;
	{ /// Update inputs
		if (keyPressed(KEY_LEFT)) inputLeft = true;
		if (keyPressed(KEY_RIGHT)) inputRight = true;
		if (keyPressed(KEY_UP)) inputUp = true;
		if (keyPressed(KEY_DOWN)) inputDown = true;
		if (keyJustPressed(' ')) inputJump = true;
		if (keyJustPressed('Z')) inputAttack = true;

		if (keyJustPressed('-')) game->timeScale /= 2.0;
		if (keyJustPressed('=')) game->timeScale *= 2.0;
	}

	/// Sort units
	Unit *activeUnits[UNIT_LIMIT];
	int activeUnitsNum = 0;

	for (int i = 0; i < UNIT_LIMIT; i++) {
		Unit *unit = &game->units[i];
		if (unit->exists) activeUnits[activeUnitsNum++] = unit;
	}

	qsort(activeUnits, activeUnitsNum, sizeof(Unit *), qsortActiveUnits);

	/// Update units
	for (int i = 0; i < activeUnitsNum; i++) {
		Unit *unit = activeUnits[i];
		if (!unit->exists) continue;

		unit->moveAccel.setTo();

		{ /// Player control
			if (unit->isPlayer) {
				Point moveVec = {};
				if (inputUp) {
					moveVec.y = -1;
				}
				if (inputDown) {
					moveVec.y = 1;
				}
				if (inputLeft) {
					moveVec.x = -1;
				}
				if (inputRight) {
					moveVec.x = 1;
				}

				moveVec.normalize(1);
				unit->moveAccel = moveVec;
			}
		}

		{ /// AI control
			if (!unit->isPlayer) {
				int moveToX = unit->memory[0];
				int moveToY = unit->memory[1];
				int waitFrames = unit->memory[2];

				if (distanceBetween(unit->x, unit->y, moveToX, moveToY) <= 10) {
					moveToX = 0;
					moveToY = 0;
				}

				if (moveToX == 0 && moveToY == 0) {
					waitFrames--;
					if (waitFrames <= 0) {
						waitFrames = rndInt(60, 120);
						moveToX = rndInt(100, platform->windowWidth - 100);
						moveToY = rndInt(100, platform->windowHeight - 100);
					}
				} else {
					Point moveVec = vectorBetween(unit->x, unit->y, moveToX, moveToY);
					moveVec.normalize();
					unit->moveAccel = moveVec;

					drawCircle(moveToX, moveToY, 4, 0xFFFF0000);
				}

				unit->memory[0] = moveToX;
				unit->memory[1] = moveToY;
				unit->memory[2] = waitFrames;
			}
		}

		{ /// Movement animation
			if (unit->moveAccel.x < 0) {
				unit->facingRight = false;
				if (unit->state == unit->idleLeft || unit->state == unit->walkLeft) {
					unit->state = unit->walkLeft;
				} else {
					unit->state = unit->transRightToLeft;
					if (getAnimFramesInState(unit) == unit->currentAnim->framesNum) unit->state = unit->walkLeft;
				}
			}

			else if (unit->moveAccel.x > 0) {
				unit->facingRight = true;
				if (unit->state == unit->idleRight || unit->state == unit->walkRight) {
					unit->state = unit->walkRight;
				} else {
					unit->state = unit->transLeftToRight;
					if (getAnimFramesInState(unit) == unit->currentAnim->framesNum) unit->state = unit->walkRight;
				}
			}

			else if (unit->moveAccel.y != 0) {
				unit->state = unit->facingRight ? unit->walkRight : unit->walkLeft;
			}

			else if (unit->moveAccel.isZero()) {
				unit->state = unit->facingRight ? unit->idleRight : unit->idleLeft;
			}
		}

		{ /// Update physics
			Point veloReduce = unit->velo;
			veloReduce.multiply(&unit->drag);
			veloReduce.multiply(game->timeScale);
			unit->velo.subtract(&veloReduce);

			unit->accel.setTo();
			unit->accel.add(&unit->moveAccel);

			unit->velo.add(&unit->accel);
		}

		{ /// Collision
			int maxIntegrations = 5;
			for (int integrateI = 0; integrateI < maxIntegrations; integrateI++) {
				unit->x += unit->velo.x/maxIntegrations * game->timeScale;
				unit->y += unit->velo.y/maxIntegrations * game->timeScale;

				for (int tileI = 0; tileI < game->tilesWide*game->tilesHigh; tileI++) {
					if (game->collTiles[tileI] == 0) continue;
					Rect tile;
					tile.setTo((tileI % game->tilesWide) * game->tileSize, (tileI / game->tilesWide) * game->tileSize, game->tileSize, game->tileSize);

					float bump = 1.0/maxIntegrations;

					while (tile.containsPoint(unit->x - unit->collWidth/2, unit->y - unit->collHeight/2)) { // Left
						unit->x += bump;
						unit->velo.x = 0;
					}

					while (tile.containsPoint(unit->x + unit->collWidth/2, unit->y - unit->collHeight/2)) { // Right
						unit->x -= bump;
						unit->velo.x = 0;
					}

					while (tile.containsPoint(unit->x, unit->y)) { // Bot
						unit->y -= bump;
						unit->velo.y = 0;
					}

					while (tile.containsPoint(unit->x, unit->y - unit->collHeight)) { // Top
						unit->y += bump;
						unit->velo.y = 0;
					}
				}
			}
		}

		{ /// Post update
			if (unit->prevState != unit->state) {
				unit->frameAnimStarted = game->frameCount;
				unit->prevState = unit->state;
			}
		}

		{ /// Rendering
			unit->currentAnim = game->anims[unit->state];
			if (unit->currentAnim) { /// Draw frame
				int framesIn = getRealTimeFramesInState(unit);
				Frame *frame = &game->spriteFrames[unit->currentAnim->frames[getCurrentAnimFrame(unit)]];
				drawFrame(frame, unit->x, unit->y, false);
			}

			/// Debug
			// drawCircle(unit->x, unit->y, 4, 0xFF00FF00);
			// drawRect(unit->x - unit->collWidth/2, unit->y - unit->collHeight, unit->collWidth, unit->collHeight, 0x88FF0000);
		}
	}

	{ /// Update camera
		Rect *cam = &renderer->camera;
		float zoom = 1;
		cam->width = platform->windowWidth / zoom;
		cam->height = platform->windowHeight / zoom;
		// cam->x = game->player->x - cam->width/2;
		// cam->y = game->player->y + game->player->collHeight/2 - cam->height/2;
	}

	swapBuffers();

	game->frameCount++;
}

void drawFrame(Frame *frame, float xpos, float ypos, bool flipX) {
	RenderProps props = newRenderProps();
	props.srcX = frame->srcX;
	props.srcY = frame->srcY;
	props.srcWidth = frame->srcWidth;
	props.srcHeight = frame->srcHeight;
	props.localX = frame->destOffX;
	props.localY = frame->destOffY;
	props.pivotX = frame->destWidth/2;
	props.pivotY = frame->destHeight/2;
	props.x = xpos - frame->destWidth/2;
	props.y = ypos - frame->destHeight;
	props.scaleX = flipX ? -1 : 1;
	drawTexture(game->sprites, &props);
}

Unit *newUnit(UnitType type) {
	for (int i = 0; i < UNIT_LIMIT; i++) {
		Unit *unit = &game->units[i];
		if (!unit->exists) {
			memset(unit, 0, sizeof(Unit));
			unit->exists = true;
			unit->type = type;
			unit->collWidth = 32;
			unit->collHeight = 32;
			unit->drag.setTo(0.2, 0.2);
			if (type == UNIT_BK) {
				unit->idleLeft = STATE_BK_IDLE_LEFT;
				unit->idleRight = STATE_BK_IDLE_RIGHT;
				unit->walkLeft = STATE_BK_WALK_LEFT;
				unit->walkRight = STATE_BK_WALK_RIGHT;
				unit->transLeftToRight = STATE_BK_TRANS_LEFT_TO_RIGHT;
				unit->transRightToLeft = STATE_BK_TRANS_RIGHT_TO_LEFT;
			} else if (type == UNIT_RM) {
				unit->idleLeft = STATE_RM_IDLE_LEFT;
				unit->idleRight = STATE_RM_IDLE_RIGHT;
				unit->walkLeft = STATE_RM_WALK_LEFT;
				unit->walkRight = STATE_RM_WALK_RIGHT;
				unit->transLeftToRight = STATE_RM_TRANS_LEFT_TO_RIGHT;
				unit->transRightToLeft = STATE_RM_TRANS_RIGHT_TO_LEFT;
			}
			unit->state = unit->idleRight;
			unit->currentAnim = game->anims[unit->state];
			return unit;
		}
	}

	return NULL;
}

int getRealTimeFramesInState(Unit *unit, int atFrame) {
	if (unit->prevState != unit->state) return 0;
	if (atFrame == -1) atFrame = game->frameCount;
	return (atFrame - unit->frameAnimStarted) * game->timeScale;
}

int getAnimFramesInState(Unit *unit, int atFrame) {
	return getRealTimeFramesInState(unit, atFrame) * unit->currentAnim->speed;
}

int getCurrentAnimFrame(Unit *unit, int atFrame) {
	int framesIn = getAnimFramesInState(unit, atFrame);

	if (unit->currentAnim->loops) {
		framesIn = framesIn % unit->currentAnim->framesNum;
	} else {
		if (framesIn > unit->currentAnim->framesNum - 1) framesIn = unit->currentAnim->framesNum - 1;
	}

	return framesIn;
}

int qsortActiveUnits(const void *a, const void *b) {
	Unit *unit1 = *(Unit **)a;
	Unit *unit2 = *(Unit **)b;

	return unit1->y - unit2->y;
}
