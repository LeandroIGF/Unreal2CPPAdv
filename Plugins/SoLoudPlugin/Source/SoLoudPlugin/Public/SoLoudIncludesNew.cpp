// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"

#ifndef WITH_SDL2_STATIC
	#define WITH_SDL2_STATIC 1
#endif

THIRD_PARTY_INCLUDES_START
#pragma warning(push) // Disabilita i warning per i file di terze parti
#pragma warning(disable : 4456 4457 4005 4996) // Unreferenced formal parameter (utile per i callback)

// Pulizia macro conflittuali
#ifdef NOSOUND
	#undef NOSOUND
#endif
#ifdef SAFE_RELEASE
	#undef SAFE_RELEASE
#endif

//#include "SDL.h"


#define _CRT_SECURE_NO_WARNINGS // Disabilita i warning per funzioni deprecate come fopen, etc. (utile per dr_wav)

// --- 1. CORE ENGINE (Assicurati che finiscano tutti in .cpp) ---
#include "soloud.cpp"
#include "soloud_audiosource.cpp"
#include "soloud_bus.cpp"
#include "soloud_core_3d.cpp"
#include "soloud_core_basicops.cpp"
#include "soloud_core_faderops.cpp"
#include "soloud_core_filterops.cpp"
#include "soloud_core_getters.cpp"
#include "soloud_core_setters.cpp"   // Risolve LNK: setVoicePan_internal
#include "soloud_core_voicegroup.cpp"
#include "soloud_core_voiceops.cpp"    // Risolve LNK: play, stopAll, stopAudioSource
#include "soloud_fader.cpp"
#include "soloud_fft.cpp"              // Risolve LNK: fft1024
#include "soloud_fft_lut.cpp"
#include "soloud_file.cpp"
#include "soloud_filter.cpp"
#include "soloud_misc.cpp"
#include "soloud_queue.cpp"
#include "soloud_thread.cpp"

// --- 2. BACKENDS (Assicurati che finiscano tutti in .cpp) ---
#include "soloud_sdl2_static.cpp"


// Nota: stb_vorbis è speciale e lo includiamo come .c
#include "stb_vorbis.c"

// --- 3. WAVE GENERATORS (Assicurati che finiscano tutti in .cpp) ---
// Queste macro attivano l'implementazione effettiva del codice dentro gli header
#define DR_WAV_IMPLEMENTATION
#define DR_MP3_IMPLEMENTATION
#define DR_FLAC_IMPLEMENTATION
#include "soloud_wav.cpp"
#include "soloud_wavstream.cpp"

#pragma warning(pop) // Ripristina i warning dopo i file di terze parti

THIRD_PARTY_INCLUDES_END

#include "Windows/HideWindowsPlatformTypes.h"
