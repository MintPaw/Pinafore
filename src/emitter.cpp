struct Particle {
	bool exists;

	Vec2 pos;
	int colour;
	float scale;
	float alpha;
	float elapsed;
	float lifetime;

	Vec2 startVelo;
	Vec2 endVelo;

	float scale0;
	float scale1;
};

struct Range {
	float min;
	float max;
};

struct Emitter {
	bool exists;
	bool explode;
	float emitRate; //@incomplete

	float x;
	float y;

	Range angle;
	Range speed0, speed1;
	Range scale0, scale1;
	Range lifetime;
	float fadeInTime;
	float fadeOutTime;

	char frameName[256];

	int startColour;
	int endColour;

	Particle *particles;
	int particlesMax;
	int particlesActive;
	int particlesToEmit;
};

Emitter *emitters = NULL;
int emittersMax = 0;

void initEmitters(int max);
Emitter *createEmitter(int maxParticles);
void updateEmitters(float elapsed);
void emit(Emitter *emitter);

void attachEmitterToCatalog(Catalog *cat, Emitter *emitter, const char *path);

void initEmitters(int max) {
	emittersMax = max;
	emitters = (Emitter *)malloc(sizeof(Emitter) * max);
	memset(emitters, 0, sizeof(Emitter) * max);
}

Emitter *createEmitter(int maxParticles) {
	for (int i = 0; i < emittersMax; i++) {
		Emitter *emitter = &emitters[i];
		if (emitter->exists) continue;

		memset(emitter, 0, sizeof(Emitter));
		emitter->exists = true;

		emitter->particles = (Particle *)malloc(sizeof(Particle) * maxParticles);
		memset(emitter->particles, 0, sizeof(Particle) * maxParticles);
		emitter->particlesMax = maxParticles;
		emitter->particlesToEmit = maxParticles;

		emitter->angle.min = 0;
		emitter->angle.max = 360;

		emitter->lifetime.min = 1;
		emitter->lifetime.min = 3;
		return emitter;
	}

	return NULL;
}

Emitter *cloneEmitter(Emitter *other) {
	Emitter *emitter = createEmitter(other->particlesMax);
		emitter->angle = other->angle;
		emitter->speed0 = other->speed0;
		emitter->speed1 = other->speed1;
		emitter->scale0 = other->scale0;
		emitter->scale1 = other->scale1;

		emitter->lifetime = other->lifetime;
		emitter->startColour = other->startColour;
		emitter->endColour = other->endColour;
		emitter->fadeInTime = other->fadeInTime;
		emitter->fadeOutTime = other->fadeOutTime;
		strcpy(emitter->frameName, other->frameName);
		return emitter;
}

void updateEmitters(float timeScale) {
	float elapsed = 1/60.0 * timeScale;
	for (int i = 0; i < emittersMax; i++) {
		Emitter *emitter = &emitters[i];
		if (!emitter->exists) continue;

		if (emitter->explode) {
			while (emitter->particlesToEmit > 0) emit(emitter);
		} else {
			// emit slowly
		}

		for (int i = 0; i < emitter->particlesMax; i++) {
			Particle *particle = &emitter->particles[i];
			if (!particle->exists) continue;
			float perc = particle->elapsed / particle->lifetime;

			Vec2 curVelo = lerpVec2(&particle->startVelo, &particle->endVelo, perc);
			curVelo.multiply(timeScale);
			particle->pos.add(&curVelo);

			particle->colour = lerpColour(emitter->startColour, emitter->endColour, perc);
			particle->scale = lerp(particle->scale0, particle->scale1, perc);

			if (particle->elapsed < emitter->fadeInTime) {
				particle->alpha = particle->elapsed/emitter->fadeInTime;
			} else if (particle->elapsed > particle->lifetime - emitter->fadeOutTime) {
				particle->alpha = (particle->lifetime - particle->elapsed)/emitter->fadeOutTime;
			} else {
				particle->alpha = 1;
			}

			particle->elapsed += elapsed;
			if (particle->elapsed >= particle->lifetime) {
				particle->exists = false;
				emitter->particlesActive--;
			}
		}

		if (emitter->particlesActive <= 0 && emitter->particlesToEmit <= 0) {
			free(emitter->particles);
			emitter->exists = false;
		}
	}
}

void emit(Emitter *emitter) {
	for (int i = 0; i < emitter->particlesMax; i++) {
		Particle *particle = &emitter->particles[i];
		if (particle->exists) continue;

		particle->exists = true;
		particle->pos.x = emitter->x;
		particle->pos.y = emitter->y;

		float angle = rndFloat(emitter->angle.min, emitter->angle.max);
		float speed0 = rndFloat(emitter->speed0.min, emitter->speed0.max);
		float speed1 = rndFloat(emitter->speed1.min, emitter->speed1.max);

		particle->startVelo.x = cos(toRad(angle)) * speed0;
		particle->endVelo.x = cos(toRad(angle)) * speed1;
		particle->startVelo.y = sin(toRad(angle)) * speed0;
		particle->endVelo.y = sin(toRad(angle)) * speed1;

		particle->scale0 = rndFloat(emitter->scale0.min, emitter->scale0.max);
		particle->scale1 = rndFloat(emitter->scale1.min, emitter->scale1.max);

		particle->lifetime = rndFloat(emitter->lifetime.min, emitter->lifetime.max);
	}

	emitter->particlesActive++;
	emitter->particlesToEmit--;
}

void attachEmitterToCatalog(Catalog *cat, Emitter *emitter, const char *path) {
	initCatalog(cat, path);
	watchFile(path);
	attachInt(cat, &emitter->startColour, "startColour");
	attachInt(cat, &emitter->endColour, "endColour");
	attachFloat(cat, &emitter->lifetime.min, "lifetimeMin");
	attachFloat(cat, &emitter->lifetime.max, "lifetimeMax");
	attachInt(cat, &emitter->particlesMax, "particlesMax");

	attachFloat(cat, &emitter->angle.min, "angleMin");
	attachFloat(cat, &emitter->angle.max, "angleMax");

	attachFloat(cat, &emitter->speed0.min, "speed0Min");
	attachFloat(cat, &emitter->speed0.max, "speed0Max");
	attachFloat(cat, &emitter->speed1.min, "speed1Min");
	attachFloat(cat, &emitter->speed1.max, "speed1Max");

	attachFloat(cat, &emitter->scale0.min, "scale0Min");
	attachFloat(cat, &emitter->scale0.max, "scale0Max");
	attachFloat(cat, &emitter->scale1.min, "scale1Min");
	attachFloat(cat, &emitter->scale1.max, "scale1Max");

	attachFloat(cat, &emitter->fadeInTime, "fadeInTime");
	attachFloat(cat, &emitter->fadeOutTime, "fadeOutTime");

	attachString(cat, emitter->frameName, "frameName");
}
