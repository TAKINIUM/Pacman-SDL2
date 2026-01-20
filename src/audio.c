#include "audio.h"
#include <SDL.h>
#include <stdio.h>

static SDL_AudioDeviceID deviceId = 0;
static Uint8 *wav_buffer_intro = NULL;
static Uint32 wav_length_intro = 0;
static Uint8 *wav_buffer_loop = NULL;
static Uint32 wav_length_loop = 0;

static int audio_volume = SDL_MIX_MAXVOLUME; // 128

void set_audio_volume(int vol) {
    if (vol < 0) vol = 0;
    if (vol > SDL_MIX_MAXVOLUME) vol = SDL_MIX_MAXVOLUME;
    audio_volume = vol;
}

void queue_audio_with_volume(Uint8* buffer, Uint32 length, SDL_AudioSpec* spec) {
    if (deviceId == 0 || !buffer || length == 0) return;
    
    // Allocate temp buffer
    Uint8* mix_buf = (Uint8*)malloc(length);
    if (!mix_buf) return;
    
    // Initialize with silence or copy? SDL_MixAudio writes OVER buffer or Adds.
    // SDL_MixAudioFormat mixes source into destination.
    // But we just want volume adjustment.
    // Clean way: memset 0 then Mix? No, Mix adds.
    // We can just use SDL_MixAudioFormat with source as buffer?
    // "This function mixes the audio data ... into the destination buffer."
    // If destination is silence, it effectively controls volume.
    memset(mix_buf, 0, length);
    SDL_MixAudioFormat(mix_buf, buffer, spec->format, length, audio_volume);
    
    SDL_QueueAudio(deviceId, mix_buf, length);
    free(mix_buf);
}

static int audio_state = 0; // 0 = intro || 1 = loop || 2 = erreurs/mute
static SDL_AudioSpec current_spec;

void init_audio() {
    SDL_AudioSpec wav_spec_intro;
    SDL_AudioSpec wav_spec_loop;

    // chargement intro
    if (SDL_LoadWAV("assets/intro.wav", &wav_spec_intro, &wav_buffer_intro, &wav_length_intro) == NULL) {
        printf("AUDIO ERREUR: Impossible de charger assets/intro.wav: %s\n", SDL_GetError());
        audio_state = 2;
    }
    
    // chargement loop
    if (SDL_LoadWAV("assets/siren.wav", &wav_spec_loop, &wav_buffer_loop, &wav_length_loop) == NULL) {
         printf("AUDIO INFO: assets/siren.wav introuvable, essai de assets/loop.wav...\n");
        if (SDL_LoadWAV("assets/loop.wav", &wav_spec_loop, &wav_buffer_loop, &wav_length_loop) == NULL) {
             printf("AUDIO ERREUR: Impossible de charger assets/loop.wav aussi.\n");
             audio_state = 2; // coupure de son si pas les audios
        }
    }

    if (audio_state != 2) {
        // audio device
        deviceId = SDL_OpenAudioDevice(NULL, 0, &wav_spec_intro, NULL, 0);
        
        if (deviceId == 0) {
            printf("AUDIO ERREUR: Echec ouverture Audio Device: %s\n", SDL_GetError());
            audio_state = 2;
        } else {
            // intro
            if (wav_buffer_intro && wav_length_intro > 0) {
                current_spec = wav_spec_intro; // IMPORTANT: Save spec for volume mixing
                queue_audio_with_volume(wav_buffer_intro, wav_length_intro, &current_spec);
                printf("AUDIO: Playing Intro...\n");
            }
            SDL_PauseAudioDevice(deviceId, 0); // Unpause
            audio_state = 0; 
        }
    }
}

void update_audio() {
    if (audio_state == 2 || deviceId == 0) return;

    Uint32 queued = SDL_GetQueuedAudioSize(deviceId);

    // intro
    if (audio_state == 0) {
        // loop trigger slightly before end to avoid gap
        if (queued < 8192) { // Pre-buffer if running low
            if (wav_buffer_loop && wav_length_loop > 0) {
                audio_state = 1;
                queue_audio_with_volume(wav_buffer_loop, wav_length_loop, &current_spec);
            }
        }
    } 
    // loop
     else if (audio_state == 1) {
        // reloop - keep buffer filled
        if (queued < wav_length_loop) {
             queue_audio_with_volume(wav_buffer_loop, wav_length_loop, &current_spec);
        }
    }
}

// kill audio
void clean_audio() {
    if (deviceId != 0) {
        SDL_CloseAudioDevice(deviceId);
        deviceId = 0;
    }
    if (wav_buffer_intro) {
        SDL_FreeWAV(wav_buffer_intro);
        wav_buffer_intro = NULL;
    }
    if (wav_buffer_loop) {
        SDL_FreeWAV(wav_buffer_loop);
        wav_buffer_loop = NULL;
    }
}