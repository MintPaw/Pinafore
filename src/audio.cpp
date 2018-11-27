#define AUDIO_AL
// #define AUDIO_DIRECT_SOUND

#include "stb_vorbis.c"

#ifdef AUDIO_AL
# include <AL/al.h>
# include <AL/alc.h>
#endif

#ifdef AUDIO_DIRECT_SOUND
# include <mmsystem.h>
# include <dsound.h>

# define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef  DIRECT_SOUND_CREATE(direct_sound_create);
#endif

#ifdef __EMSCRIPTEN__
#define SAMPLE_BUFFER_LIMIT (4096*2)
#else
#define SAMPLE_BUFFER_LIMIT (4096)
#endif
#define CHANNEL_LIMIT 512
#define SOUND_LIMIT 1024
#define SAMPLE_RATE 44100

struct Sound {
	bool exists;

	float tweakVolume;
	float length; // In seconds

	int16 *samples;
	int samplesStreamed;
	int sampleRate;
	int samplesTotal;

	stb_vorbis_alloc vorbisTempMem;
	stb_vorbis *vorbis;
};

struct Channel {
	bool exists;

	Sound *sound;
	int samplePosition;

	float userVolume;
	float tweakVolume;
	bool playing;
	bool looping;

	/// Private
	float lastVolume;
};

struct Audio {
	bool disabled;

	Channel channels[CHANNEL_LIMIT];
	Sound sounds[SOUND_LIMIT];

	float masterVolume;

	int memoryUsed;

#ifdef AUDIO_AL
	ALCdevice *device;
	ALCcontext *context;

	ALuint buffers[2];
	ALuint source;
#endif

#ifdef AUDIO_DIRECT_SOUND
	LPDIRECTSOUNDBUFFER secondaryBuffer;
	int bytesPerSample;
	int bufferSize;
#endif
};

Audio *audio = NULL;

void initAudio();

Sound *loadSound(const char *path, float tweakVolume=1);
Channel *playSound(Sound *sound, bool looping=false);
Channel *playRndSound(Sound *sound0=NULL, Sound *sound1=NULL, Sound *sound2=NULL, Sound *sound3=NULL, Sound *sound4=NULL, Sound *sound5=NULL, Sound *sound6=NULL, Sound *sound7=NULL);

void updateAudio();

/// Private
void mixSound(int16 *destBuffer, int destSamplesNum);
float computeChannelVolume(Channel *channel);
void checkAudioError(int lineNum);
#define CheckAudioError() checkAudioError(__LINE__);

void initAudio() {
	printf("Initing audio\n");

	audio = (Audio *)malloc(sizeof(Audio));
	memset(audio, 0, sizeof(Audio));
	audio->masterVolume = 1;

#ifdef AUDIO_AL
	audio->device = alcOpenDevice(NULL);
	if (!audio->device) {
		audio->disabled = true;
		printf("OpenAL failed to init! Audio disabled\n");
		return;
	}

	audio->context = alcCreateContext(audio->device, NULL);
	Assert(alcMakeContextCurrent(audio->context));

	alGenSources(1, &audio->source);
	alGenBuffers(2, audio->buffers);

	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alSourcef(audio->source, AL_PITCH, 1);
	alSourcef(audio->source, AL_GAIN, 1);
	alSourcei(audio->source, AL_LOOPING, AL_FALSE);

	int16 startingBuffer[SAMPLE_BUFFER_LIMIT] = {};
	alBufferData(audio->buffers[0], AL_FORMAT_STEREO16, startingBuffer, SAMPLE_BUFFER_LIMIT * sizeof(int16), SAMPLE_RATE);
	alBufferData(audio->buffers[1], AL_FORMAT_STEREO16, startingBuffer, SAMPLE_BUFFER_LIMIT * sizeof(int16), SAMPLE_RATE);
	CheckAudioError();

	alSourceQueueBuffers(audio->source, 2, audio->buffers);

	alSourcePlay(audio->source);

	int isPlaying;
	alGetSourcei(audio->source, AL_SOURCE_STATE, &isPlaying);

	CheckAudioError();
#endif

#ifdef AUDIO_DIRECT_SOUND
	HMODULE directSoundDll = LoadLibraryA("dsound.dll");
	if (!directSoundDll) {
		audio->disabled = true;
		printf("DirectSound failed to be loaded! Audio disabled\n");
		return;
	}

	direct_sound_create *directSoundCreate = (direct_sound_create * )GetProcAddress(directSoundDll, "DirectSoundCreate");

	LPDIRECTSOUND directSound;
	if (SUCCEEDED(directSoundCreate(0, &directSound, 0))) {
		HWND hwnd = GetActiveWindow();

		audio->bytesPerSample = sizeof(int16) * 2;
		audio->bufferSize = SAMPLE_RATE * audio->bytesPerSample;

		WAVEFORMATEX waveFormat = {};
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = SAMPLE_RATE;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

		if (SUCCEEDED(directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY))) {
			DSBUFFERDESC bufferDesc = {};
			bufferDesc.dwSize = sizeof(bufferDesc);
			bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

			LPDIRECTSOUNDBUFFER primaryBuffer;
			if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDesc, &primaryBuffer, 0))) {
				HRESULT error = primaryBuffer->SetFormat(&waveFormat);
				if (SUCCEEDED(error)) {
					printf("Created first buf!\n");
				}
			} else {
				Assert(0);
			}
		}

		{
			DSBUFFERDESC bufferDesc = {};
			bufferDesc.dwSize = sizeof(bufferDesc);
			bufferDesc.dwFlags = 0;
			bufferDesc.dwBufferBytes = audio->bufferSize;
			bufferDesc.lpwfxFormat = &waveFormat;
			if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDesc, &audio->secondaryBuffer, 0))) {
				printf("Created second buf!\n");
			} else {
				Assert(0);
			}
		}

		audio->secondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
	} else {
		audio->disabled = true;
		printf("DirectSound failed to init! Audio disabled\n");
		return;
	}
#endif
}

Sound *loadSound(const char *path, float tweakVolume) {
	Sound *sound = NULL;
	for (int i = 0; i < SOUND_LIMIT; i++) {
		if (!audio->sounds[i].exists) {
			sound = &audio->sounds[i];
			break;
		}
	}

	if (!sound) {
		printf("There are no more sound slots\n");
		Assert(0);
	}

	memset(sound, 0, sizeof(Sound));
	sound->exists = true;

	sound->tweakVolume = tweakVolume;
	sound->vorbisTempMem.alloc_buffer = (char *)malloc(500*1024);
	sound->vorbisTempMem.alloc_buffer_length_in_bytes = 500*1024;

	int oggLen;
	unsigned char *oggData = (unsigned char *)readFile(path, &oggLen);

	int err;
	sound->vorbis = stb_vorbis_open_memory(oggData, oggLen, &err, &sound->vorbisTempMem);

	if (err != 0) {
		printf("Stb vorbis failed to start stream with error %d\n", err);
		Assert(0);
	}

	sound->length = stb_vorbis_stream_length_in_seconds(sound->vorbis);
	sound->sampleRate = sound->vorbis->sample_rate;
	sound->samplesTotal = sound->vorbis->total_samples * 2;
	sound->samples = (int16 *)malloc(sound->samplesTotal * sizeof(float));
	audio->memoryUsed += sound->samplesTotal * sizeof(float);
	// printf("Using %0.2fmb of sound memory\n", audio->memoryUsed / 1024.0 / 1024.0);

	return sound;
}

Channel *playSound(Sound *sound, bool looping) {
	Channel *channel = NULL;
	int totalChannelCount = 0;
	int soundCount = 0;
	for (int i = 0; i < CHANNEL_LIMIT; i++) {
		if (audio->channels[i].exists) {
			totalChannelCount++;
			if (audio->channels[i].sound == sound) soundCount++;
		} else {
			if (!channel) channel = &audio->channels[i];
		}
	}

	if (soundCount >= 5) return NULL;
	if (totalChannelCount >= 30 && sound->length < 1) return NULL;

	if (!channel) {
		printf("There are no more sound channels!\n");
	}

	memset(channel, 0, sizeof(Channel));

	channel->userVolume = 1;
	channel->tweakVolume = sound->tweakVolume;
	channel->playing = true;
	channel->exists = true;
	channel->looping = looping;

	channel->lastVolume = computeChannelVolume(channel);
	channel->sound = sound;

	return channel;
}

Channel *playRndSound(Sound *sound0, Sound *sound1, Sound *sound2, Sound *sound3, Sound *sound4, Sound *sound5, Sound *sound6, Sound *sound7) {
	Sound *options[8];
	int optionsNum = 0;

	if (sound0) options[optionsNum++] = sound0;
	if (sound1) options[optionsNum++] = sound1;
	if (sound2) options[optionsNum++] = sound2;
	if (sound3) options[optionsNum++] = sound3;
	if (sound4) options[optionsNum++] = sound4;
	if (sound5) options[optionsNum++] = sound5;
	if (sound6) options[optionsNum++] = sound6;
	if (sound7) options[optionsNum++] = sound7;

	return playSound(options[rndInt(0, optionsNum-1)]);
}

void updateAudio() {
	if (audio->disabled) return;

#ifdef AUDIO_AL
	int toProcess;
	alGetSourcei(audio->source, AL_BUFFERS_PROCESSED, &toProcess);

	bool requeued = false;
	for(int i = 0; i < toProcess; i++) {
		requeued = true;

		ALuint buffer;
		alSourceUnqueueBuffers(audio->source, 1, &buffer);

		int16 shortBuffer[SAMPLE_BUFFER_LIMIT];
		mixSound(shortBuffer, SAMPLE_BUFFER_LIMIT);

#if 0
		float freq = 440.f;
		for (int i = 0; i < SAMPLE_BUFFER_LIMIT; i++) {
			shortBuffer[i] = 32760 * sin( (2.f*float(M_PI)*freq)/SAMPLE_RATE * i );
		}
#endif

		alBufferData(buffer, AL_FORMAT_STEREO16, shortBuffer, SAMPLE_BUFFER_LIMIT * sizeof(int16), SAMPLE_RATE);
		CheckAudioError();

		alSourceQueueBuffers(audio->source, 1, &buffer);
		CheckAudioError();
	}

	if (requeued) {
		int isPlaying;
		alGetSourcei(audio->source, AL_SOURCE_STATE, &isPlaying);
		if (isPlaying == AL_STOPPED) alSourcePlay(audio->source);
	}
#endif

#ifdef AUDIO_DIRECT_SOUND
	static uint32 runningSampleIndex = 0;

	DWORD playCursor;
	DWORD writeCursor;
	audio->secondaryBuffer->GetCurrentPosition(&playCursor, &writeCursor);
	// printf("Samples ahead: %d\n", writeCursor);

	DWORD byteToLock = (runningSampleIndex*audio->bytesPerSample) % audio->bufferSize;
	DWORD bytesToWrite;
	if (byteToLock > playCursor) {
		bytesToWrite = audio->bufferSize - byteToLock;
		bytesToWrite += playCursor;
	} else {
		bytesToWrite = playCursor - byteToLock;
	}

	VOID *region1;
	DWORD region1Size;
	VOID *region2;
	DWORD region2Size;
	audio->secondaryBuffer->Lock(byteToLock, bytesToWrite, &region1, &region1Size, &region2, &region2Size, 0);

	const int samplesNeeded = SAMPLE_RATE / 60 * 2;
	int16 shortBuffer[samplesNeeded];
	mixSound(shortBuffer, samplesNeeded);

	int samplesUsed = 0;
	int16 *sampleOut = (int16 *)region1;
	DWORD region1SampleCount = region1Size/audio->bytesPerSample;
	for (DWORD sampleIndex = 0; sampleIndex < region1SampleCount; sampleIndex++) {
#if 0
		int squareWavePeriod = SAMPLE_RATE/256;
		int volume = 3000;
		int sampleValue = (runningSampleIndex / (squareWavePeriod/2)) % 2 ? volume : -volume;
		*sampleOut++ = sampleValue;
		*sampleOut++ = sampleValue;
#else
		if (samplesUsed >= samplesNeeded) break;
		*sampleOut++ = shortBuffer[samplesUsed++];
		*sampleOut++ = shortBuffer[samplesUsed++];
#endif
		runningSampleIndex++;
	}

	sampleOut = (int16 *)region2;
	DWORD region2SampleCount = region2Size/audio->bytesPerSample;
	for (DWORD sampleIndex = 0; sampleIndex < region2SampleCount; sampleIndex++) {
#if 0
		int squareWavePeriod = SAMPLE_RATE/256;
		int volume = 3000;
		int sampleValue = (runningSampleIndex / (squareWavePeriod/2)) % 2 ? volume : -volume;
		*sampleOut++ = sampleValue;
		*sampleOut++ = sampleValue;
#else
		if (samplesUsed >= samplesNeeded) break;
		*sampleOut++ = shortBuffer[samplesUsed++];
		*sampleOut++ = shortBuffer[samplesUsed++];
#endif
		runningSampleIndex++;
	}

	audio->secondaryBuffer->Unlock(region1, region1Size, region2, region2Size);
#endif

	CheckAudioError();
}

void mixSound(int16 *destBuffer, int destSamplesNum) {
	int16 sampleBuffer[SAMPLE_BUFFER_LIMIT];
	memset(sampleBuffer, 0, destSamplesNum * sizeof(int16));

	for (int i = 0; i < CHANNEL_LIMIT; i++) {
		Channel *channel = &audio->channels[i];
		if (!channel->exists || !channel->playing) continue;
		Sound *sound = channel->sound;

		if (sound->samplesStreamed < sound->samplesTotal) {
			int samplesGot = stb_vorbis_get_samples_short_interleaved(sound->vorbis, 2, &sound->samples[sound->samplesStreamed], destSamplesNum) * 2;
			sound->samplesStreamed += samplesGot;
			// printf("Streaming %d/%d samples\n", sound->samplesStreamed, sound->samplesTotal);

			if (samplesGot < destSamplesNum) {
				// printf("Finished streaming %d/%d samples\n", sound->samplesStreamed, sound->samplesTotal);
				stb_vorbis_close(sound->vorbis);
				free(sound->vorbisTempMem.alloc_buffer);
				sound->vorbis = NULL;
			}
		}

		channel->userVolume = Clamp(channel->userVolume, 0, 1);

		float vol = computeChannelVolume(channel);
		float startVol = vol;
		float volAdd = 0;

		if (vol != channel->lastVolume) {
			float minVol = channel->lastVolume;
			float maxVol = vol;
			startVol = minVol;
			volAdd = (maxVol-minVol) / destSamplesNum;
		}

		channel->lastVolume = vol;

		int16 samples[SAMPLE_BUFFER_LIMIT] = {};
		int samplesNum = 0;

		while (samplesNum < destSamplesNum) {
			int samplesToGet = destSamplesNum - samplesNum;
			int samplesLeft = sound->samplesTotal - channel->samplePosition;
			if (samplesToGet > samplesLeft) samplesToGet = samplesLeft;
			// printf("Playing %d samples from pos %d, %d left (%d avail)\n", samplesToGet, channel->samplePosition, samplesLeft, sound->samplesStreamed - channel->samplePosition);

			for (int i = 0; i < samplesToGet; i++) {
				samples[samplesNum++] = sound->samples[channel->samplePosition++];
			}

			if (channel->samplePosition >= sound->samplesTotal-1) {
				if (channel->looping) {
					channel->samplePosition = 0;
				} else {
					channel->exists = false;
					break;
				}
			}
		}

		for (int j = 0; j < destSamplesNum; j+=2) {
			float curVol = startVol + volAdd*j;
			sampleBuffer[j] += samples[j] * curVol;
			sampleBuffer[j+1] += samples[j+1] * curVol;
		}
	}

	for (int i = 0; i < destSamplesNum; i++) destBuffer[i] = Clamp(sampleBuffer[i], SHRT_MIN, SHRT_MAX);
}

float computeChannelVolume(Channel *channel) {
	return channel->tweakVolume * channel->userVolume * audio->masterVolume;
}

void checkAudioError(int lineNum) {
#ifdef AUDIO_AL
	ALCenum error = alGetError();

	if (error != AL_NO_ERROR) {
		printf("Openal is in error state %d (line: %d)\n", error, lineNum);
		Assert(0);
	}
#endif
}
