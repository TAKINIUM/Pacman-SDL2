#include "audio.h"
#include <SDL.h>
#include <stdio.h>

static SDL_AudioDeviceID deviceId = 0;
static Uint8 *wav_buffer_intro = NULL;
static Uint32 wav_length_intro = 0;
static Uint8 *wav_buffer_loop = NULL;
static Uint32 wav_length_loop = 0;

static int audio_state = 0; // 0 = intro || 1 = loop || 2 = erreurs/mute
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
                SDL_QueueAudio(deviceId, wav_buffer_intro, wav_length_intro);
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
        // loop
        if (queued == 0) {
            if (wav_buffer_loop && wav_length_loop > 0) {
                audio_state = 1;
                SDL_QueueAudio(deviceId, wav_buffer_loop, wav_length_loop);
                // printf("AUDIO: start loop\n");
            }
        }
    } 
    // loop
     else if (audio_state == 1) {
        // reloop
        if (queued < wav_length_loop) {
             SDL_QueueAudio(deviceId, wav_buffer_loop, wav_length_loop);
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