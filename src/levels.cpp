void getLevelQueue(int level);
const char *getString(StringId id);

int getDamage(TowerType *type, int damageLevel);
int getSlow(TowerType *type, int slowLevel);
float getRateOfFire(TowerType *type, int rateOfFireLevel);
int getRange(TowerType *type, int rangeLevel);
float getSpeed(int type);
int getHp(int type);
int getBounty(int type);
int getToolPrice(Tool type);

int getDamagePrice(int damageLevel);
int getSlowPrice(int slowLevel);
int getRateOfFirePrice(int rateOfFireLevel);
int getRangePrice(int rangeLevel);

int getUnitDamage(int unitType);

void pushDelay(float seconds);
void pushSpawn(int type);
void pushMultiSpawn(int type, int amount, float overTime);

#define TOWER1_SHOOT_ANIM_LENGTH 0.3
#define TOWER2_SHOOT_ANIM_LENGTH 0.3
#define TOWER3_SHOOT_ANIM_LENGTH 1.3

#define TOWER_1_SHOOT_DELAY_PERCENTAGE 0.85
#define TOWER_2_SHOOT_DELAY_PERCENTAGE 0.85
#define TOWER_3_SHOOT_DELAY_PERCENTAGE 0.80

int NORMAL = 1;
int TANKY = 2;
int FAST = 3;
int VAMPIRE = 4;

void getLevelQueue(int level) {
	if (level == 1) {

		pushDelay(5);
		pushMultiSpawn(NORMAL, 15, 35);

	} else if (level == 2) {

		pushDelay(5);
		pushMultiSpawn(NORMAL, 10, 15);

	} else if (level == 3) {

		pushDelay(5);
		pushMultiSpawn(NORMAL, 10, 10);
		pushDelay(12);
		pushMultiSpawn(NORMAL, 10, 5);

	} else if (level == 4) {

		pushDelay(5);
		pushMultiSpawn(FAST, 25, 15);

	} else if (level == 5) {

		pushDelay(5);
		pushMultiSpawn(TANKY, 10, 20);

	} else if (level == 6) {

		pushDelay(5);
		pushMultiSpawn(NORMAL, 15, 10);
		pushDelay(5);
		pushMultiSpawn(TANKY, 15, 20);

	} else if (level == 7) {

		pushDelay(5);
		pushMultiSpawn(FAST, 5, 4);
		pushDelay(2);
		pushMultiSpawn(NORMAL, 10, 7);
		pushDelay(2);
		pushMultiSpawn(FAST, 5, 5);
		pushDelay(1);
		pushMultiSpawn(NORMAL, 10, 10);
		pushMultiSpawn(TANKY, 15, 15);
		pushDelay(5);

	} else if (level == 8) {

		pushDelay(5);
		pushMultiSpawn(TANKY, 20, 15);
		pushMultiSpawn(FAST, 10, 5);
		pushMultiSpawn(NORMAL, 5, 3);
		pushDelay(5);
		pushMultiSpawn(NORMAL, 5, 3);
		pushDelay(5);
		pushMultiSpawn(FAST, 15, 5);
		pushMultiSpawn(NORMAL, 7, 3);
		pushDelay(3);
		pushMultiSpawn(NORMAL, 10, 3);

	} else if (level == 9) {

		pushDelay(5);
		pushMultiSpawn(VAMPIRE, 10, 20);
		pushDelay(5);
		pushMultiSpawn(NORMAL, 5, 3);
		pushDelay(3);
		pushMultiSpawn(FAST, 15, 5);
		pushDelay(2);
		pushMultiSpawn(NORMAL, 5, 3);

	} else if (level == 10) {

		pushDelay(5);
		pushMultiSpawn(VAMPIRE, 5, 10);
		pushDelay(2);
		pushMultiSpawn(FAST, 10, 5);
		pushDelay(7);
		pushMultiSpawn(VAMPIRE, 7, 10);
		pushDelay(2);
		pushMultiSpawn(FAST, 12, 7);
		pushDelay(10);
		pushMultiSpawn(FAST, 20, 10);
		pushDelay(2);
		pushMultiSpawn(VAMPIRE, 7, 10);
		

	} else if (level == 11) {

		pushDelay(5);
		pushMultiSpawn(TANKY, 20, 10);
		pushDelay(7);
		pushMultiSpawn(TANKY, 25, 15);
		pushDelay(10);
		pushMultiSpawn(TANKY, 20, 10);

	} else if (level == 12) {

		pushDelay(5);
		pushMultiSpawn(TANKY, 30, 20);
		pushDelay(10);
		pushMultiSpawn(FAST, 30, 12);
		pushDelay(10);
		pushMultiSpawn(NORMAL, 40, 12);

	} else if (level == 13) {

		pushDelay(5);
		pushMultiSpawn(FAST, 80, 30);
		pushMultiSpawn(VAMPIRE, 5, 5);
		pushDelay(2);
		pushMultiSpawn(VAMPIRE, 5, 15);
		pushDelay(15);
		pushMultiSpawn(VAMPIRE, 5, 5);
		pushDelay(2);
		pushMultiSpawn(VAMPIRE, 5, 15);
		pushDelay(10);
		pushMultiSpawn(VAMPIRE, 5, 10);
		pushDelay(7);
		pushMultiSpawn(FAST, 50, 20);

	} else if (level == 14) {

		pushDelay(5);
		pushMultiSpawn(VAMPIRE, 20, 10);
		pushMultiSpawn(TANKY, 50, 30);
		pushDelay(15);
		pushMultiSpawn(FAST, 50, 30);
		

	} else if (level == 15) {

		pushDelay(5);
		pushMultiSpawn(TANKY, 25, 7);
		pushDelay(7);
		pushMultiSpawn(VAMPIRE, 20, 16);
		pushDelay(8);
		pushMultiSpawn(VAMPIRE, 25, 18);
		pushDelay(5);
		pushMultiSpawn(NORMAL, 40, 40);
		pushMultiSpawn(FAST, 30, 30);
		pushDelay(30);
		pushMultiSpawn(TANKY, 10, 20);
		pushDelay(5);
		pushMultiSpawn(FAST, 20, 2.5);
		pushDelay(2);
		pushMultiSpawn(FAST, 12, 2.3);
		pushDelay(1);
		pushMultiSpawn(FAST, 20, 5.1);
		pushDelay(1);
		pushMultiSpawn(FAST, 20, 5.8);
		pushDelay(2);
		pushMultiSpawn(VAMPIRE, 20, 30);
		pushMultiSpawn(VAMPIRE, 25, 16);
		pushMultiSpawn(FAST, 5, 1.6);
		pushDelay(1);
		pushMultiSpawn(FAST, 4, 1.5);
		pushDelay(8);
		pushMultiSpawn(TANKY, 20, 10);
		pushDelay(2);
		pushMultiSpawn(FAST, 20, 5);
		pushDelay(3);
		pushMultiSpawn(TANKY, 20, 30);
		pushMultiSpawn(FAST, 20, 7);
		pushMultiSpawn(VAMPIRE, 30, 18);
		pushDelay(20);
		pushMultiSpawn(TANKY, 20, 20);
		pushMultiSpawn(FAST, 10, 4);
		pushMultiSpawn(FAST, 20, 6);
		pushMultiSpawn(VAMPIRE, 30, 10);
		pushMultiSpawn(NORMAL, 50, 20);
		

	} else {
		for (int i = 0; i < 999; i++) pushDelay(999999999);
	}
}

const char *getString(StringId id) {
	if (id == STRING_HAND_NAME) return "Hand";
	if (id == STRING_TOWER1_NAME) return "Rapid Magic";
	if (id == STRING_TOWER2_NAME) return "Freeze Magic";
	if (id == STRING_TOWER3_NAME) return "Bomb Magic";

	printf("Unknown string id %d\n", id);
	Assert(0);
	return NULL;
}

int getDamage(TowerType type, int damageLevel) {
	if (type == TOWER1) {
		return (damageLevel+1) * 7;
	} else if (type == TOWER2) {
		return (damageLevel+1) * 10;
	} else if (type == TOWER3) {
		return (damageLevel+2) * 8;
	}

	return 0;
}

int getSlow(TowerType type, int slowLevel) {
	if (type == TOWER2) {
		return (slowLevel+1) * 30;
	}

	return 0;
}

float getRateOfFire(TowerType type, int rateOfFireLevel) {
	if (type == TOWER1) {
		return lerp(0.65, 0.2, (float)rateOfFireLevel/10.0);
	} else if (type == TOWER2) {
		return lerp(1.7, 0.3, (float)rateOfFireLevel/10.0);
	} else if (type == TOWER3) {
		return lerp(5, 2, (float)rateOfFireLevel/10.0);
	}

	return 0;
}

int getRange(TowerType type, int rangeLevel) {
	if (type == TOWER1) {
		return lerp(60, 230, (float)rangeLevel/10.0);
	} else if (type == TOWER2) {
		return lerp(70, 300, (float)rangeLevel/10.0);
	} else if (type == TOWER3) {
		return lerp(60, 125, (float)rangeLevel/10.0);
	}

	return 0;
}

int getDamagePrice(int damageLevel) {
	return (damageLevel+1) * 200;
}

int getSlowPrice(int slowLevel) {
	return (slowLevel+1) * 100;
}

int getRateOfFirePrice(int rateOfFireLevel) {
	return (rateOfFireLevel+1) * 75;
}

int getRangePrice(int rangeLevel) {
	return (rangeLevel+1) * 200;
}

int getUnitDamage(int unitType) {
	if (unitType == NORMAL) {
		return 10;
	} else if (unitType == TANKY) {
		return 15;
	} else if (unitType == FAST) {
		return 5;
	} else if (unitType == VAMPIRE) {
		return 20;
	}

	return 1; 
}


float getSpeed(int type) {
	if (type == NORMAL) {
		return 0.9;
	} else if (type == TANKY) {
		return 0.65;
	} else if (type == FAST) {
		return 2.4;
	} else if (type == VAMPIRE) {
		return 0.5;
	}

	return 1; 
}

int getHp(int type) {
	if (type == NORMAL) {
		return 90;
	} else if (type == TANKY) {
		return 250;
	} else if (type == FAST) {
		return 40;
	} else if (type == VAMPIRE) {
		return 200;
	}

	return 9999999; 
}

int getBounty(int type) {
	if (type == NORMAL) {
		return 70;
	} else if (type == TANKY) {
		return 80;
	} else if (type == FAST) {
		return 50;
	} else if (type == VAMPIRE) {
		return 100;
	}

	printf("No money earned!\n");
	return 9999999; 
}

int getToolPrice(Tool type) {
	if (type == TOOL_TOWER1) {
		return 250;
	} else if (type == TOOL_TOWER2) {
		return 500;
	} else if (type == TOOL_TOWER3) {
		return 500;
	}

	return 9999999; 
}

void pushDelay(float seconds) {
	Event *event = &game->eventQueue[game->eventQueueNum++];
	event->type = EVENT_DELAY;
	event->delayTime = seconds;
}

void pushSpawn(int type) {
	Event *event = &game->eventQueue[game->eventQueueNum++];
	event->type = EVENT_SPAWN;
	event->unitType = type;
}

void pushMultiSpawn(int type, int amount, float overTime) {
	Event *event = &game->eventQueue[game->eventQueueNum++];
	event->type = EVENT_MULTI_SPAWN;
	event->unitType = type;
	event->amount = amount;
	event->overTime = overTime;
}
