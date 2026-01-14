#include "audio.h"
#include <SDL.h>
#include <stdio.h>

static SDL_AudioDeviceID deviceId;
static Uint8 *wav_buffer_intro = NULL;
static Uint32 wav_length_intro = 0;
static Uint8 *wav_buffer_loop = NULL;
static Uint32 wav_length_loop = 0;

static int audio_state = 0; // 0 Intro || 1 Loop || 2 Error/Done

void init_audio() {
    SDL_AudioSpec wav_spec_intro;
    SDL_AudioSpec wav_spec_loop;

    if (SDL_LoadWAV("assets/intro.wav", &wav_spec_intro, &wav_buffer_intro, &wav_length_intro) == NULL) {
        printf("Audio Warning: Could not open assets/intro.wav: %s\n", SDL_GetError());
        audio_state = 2;
        return;
    }
    
    if (SDL_LoadWAV("assets/siren.wav", &wav_spec_loop, &wav_buffer_loop, &wav_length_loop) == NULL) {
        if (SDL_LoadWAV("assets/loop.wav", &wav_spec_loop, &wav_buffer_loop, &wav_length_loop) == NULL) {
             printf("Audio Warning: Could not open assets/siren.wav or loop.wav\n");
             audio_state = 2;
        }
    }

    if (audio_state != 2) {
        deviceId = SDL_OpenAudioDevice(NULL, 0, &wav_spec_intro, NULL, 0);
        if (deviceId == 0) {
            printf("Audio Error: Failed to open audio device: %s\n", SDL_GetError());
            audio_state = 2;
        } else {
            // Play Intro
            SDL_QueueAudio(deviceId, wav_buffer_intro, wav_length_intro);
            SDL_PauseAudioDevice(deviceId, 0);
            audio_state = 0; 
        }
    }
}

void update_audio() {
    if (audio_state == 2 || deviceId == 0) return;

    // Check fin intro
    if (audio_state == 0) {
        if (SDL_GetQueuedAudioSize(deviceId) == 0) {
            // Start Loop
            audio_state = 1;
            SDL_QueueAudio(deviceId, wav_buffer_loop, wav_length_loop);
        }
    } 
    else if (audio_state == 1) {
        // Check redo loop
        if (SDL_GetQueuedAudioSize(deviceId) < wav_length_loop) {
             SDL_QueueAudio(deviceId, wav_buffer_loop, wav_length_loop);
        }
    }
}

void clean_audio() {
    if (deviceId != 0) {
        SDL_CloseAudioDevice(deviceId);
    }
    if (wav_buffer_intro) SDL_FreeWAV(wav_buffer_intro);
    if (wav_buffer_loop) SDL_FreeWAV(wav_buffer_loop);
}