#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>

// Screen dimensions and frame rate
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 800;
constexpr int FPS = 80;

// Global Allegro objects
extern ALLEGRO_DISPLAY* display;
extern ALLEGRO_EVENT_QUEUE* event_queue;
extern ALLEGRO_TIMER* timer;
extern ALLEGRO_FONT* font;
extern ALLEGRO_SAMPLE* game_sound;
extern ALLEGRO_SAMPLE_INSTANCE* game_sound_instance;

bool initialize_allegro();
bool initialize_game();
void Alma();
void cleanup_game();
void cleanup_allegro();

#endif // GAME_H