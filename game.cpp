#include "game.h"
#include "bike.h"
#include "environment.h"
#include "highway.h"
#include <iostream>

ALLEGRO_DISPLAY* display = nullptr;
ALLEGRO_EVENT_QUEUE* event_queue = nullptr;
ALLEGRO_TIMER* timer = nullptr;
ALLEGRO_FONT* font = nullptr;
ALLEGRO_SAMPLE* game_sound = nullptr;
ALLEGRO_SAMPLE_INSTANCE* game_sound_instance = nullptr;

bool initialize_allegro() {
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro core!\n";
        return false;
    }

    if (!al_init_image_addon()) {
        std::cerr << "Failed to initialize image addon!\n";
        return false;
    }

    if (!al_init_font_addon()) {
        std::cerr << "Failed to initialize font addon!\n";
        return false;
    }

    if (!al_init_ttf_addon()) {
        std::cerr << "Failed to initialize TTF addon!\n";
        return false;
    }

    if (!al_init_primitives_addon()) {
        std::cerr << "Failed to initialize primitives addon!\n";
        return false;
    }

    // Initialize audio addons
    if (!al_install_audio()) {
        std::cerr << "Failed to install audio!\n";
        return false;
    }

    if (!al_init_acodec_addon()) {
        std::cerr << "Failed to initialize audio codec addon!\n";
        return false;
    }

    // Reserve samples for audio playback
    if (!al_reserve_samples(1)) {
        std::cerr << "Failed to reserve audio samples!\n";
        return false;
    }

    // Install peripherals
    if (!al_install_keyboard()) {
        std::cerr << "Failed to install keyboard!\n";
        return false;
    }

    // Create display
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        std::cerr << "Failed to create display!\n";
        return false;
    }

    // Create event queue
    event_queue = al_create_event_queue();
    if (!event_queue) {
        std::cerr << "Failed to create event queue!\n";
        al_destroy_display(display);
        return false;
    }

    // Create timer (60 FPS)
    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        std::cerr << "Failed to create timer!\n";
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        return false;
    }

    // Register event sources
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    // Set window title
    al_set_window_title(display, "Traffic Rider");

    return true;
}

bool initialize_game() {
    // Create built-in font
    font = al_create_builtin_font();
    if (!font) {
        std::cerr << "Failed to create built-in font!\n";
        return false;
    }

    // Load game sound
    game_sound = al_load_sample("assets/gamesound.mp3");
    if (!game_sound) {
        std::cerr << "Failed to load game sound file! Make sure 'assets/gamesound.mp3' exists.\n";
        // Continue even if sound loading fails - we'll handle this gracefully
    }
    else {
        // Create a sample instance for better control
        game_sound_instance = al_create_sample_instance(game_sound);
        if (!game_sound_instance) {
            std::cerr << "Failed to create sound instance!\n";
            al_destroy_sample(game_sound);
            game_sound = nullptr;
        }
        else {
            // Configure the sound instance
            al_set_sample_instance_playmode(game_sound_instance, ALLEGRO_PLAYMODE_LOOP);
            al_attach_sample_instance_to_mixer(game_sound_instance, al_get_default_mixer());
        }
    }

    // Seed random number generator
    srand(static_cast<unsigned>(time(nullptr)));

    return true;
}

void Alma() {
    // Create game objects
    Bike bike;
    Player player(SCREEN_WIDTH / 2 - bike.getWidth() / 2,
        SCREEN_HEIGHT - bike.getHeight() - 20,
        SCREEN_HEIGHT);
    Highway highway(bike, player);

    // Game state variables
    bool running = true;
    bool redraw = true;
    bool paused = false;

    // Level up notification variables
    bool showLevelUpMessage = false;
    int levelUpMessageTimer = 0;
    int previousLevel = highway.getLevel();

    // Start the game sound if available
    if (game_sound_instance) {
        al_play_sample_instance(game_sound_instance);
    }

    // Start the game timer
    al_start_timer(timer);

    // Main game loop
    while (running && !highway.isGameOver) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        // Handle events
        switch (event.type) {
        case ALLEGRO_EVENT_TIMER:
            if (!paused) {
                // Update game state
                player.update();
                highway.update();
                highway.checkCollisions();

                // Check if level has changed
                if (highway.getLevel() > previousLevel) {
                    showLevelUpMessage = true;
                    levelUpMessageTimer = 180; // Show for 3 seconds (60 FPS * 3)
                    previousLevel = highway.getLevel();
                }

                // Update level up notification timer
                if (showLevelUpMessage) {
                    levelUpMessageTimer--;
                    if (levelUpMessageTimer <= 0) {
                        showLevelUpMessage = false;
                    }
                }

                redraw = true;
            }
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE:
                running = false;
                break;
            case ALLEGRO_KEY_LEFT:
                player.velocityX = -5; // Move left
                break;
            case ALLEGRO_KEY_RIGHT:
                player.velocityX = 5;  // Move right
                break;
            case ALLEGRO_KEY_P:
                paused = !paused;     // Toggle pause

                // Pause/resume music when game is paused/resumed
                if (game_sound_instance) {
                    if (paused) {
                        al_stop_sample_instance(game_sound_instance);
                    }
                    else {
                        al_play_sample_instance(game_sound_instance);
                    }
                }
                break;
            case ALLEGRO_KEY_M:
                // Toggle mute/unmute
                if (game_sound_instance) {
                    bool is_playing = al_get_sample_instance_playing(game_sound_instance);
                    if (is_playing) {
                        al_stop_sample_instance(game_sound_instance);
                    }
                    else if (!paused) {
                        al_play_sample_instance(game_sound_instance);
                    }
                }
                break;
            }
            break;

        case ALLEGRO_EVENT_KEY_UP:
            switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_LEFT:
            case ALLEGRO_KEY_RIGHT:
                player.velocityX = 0;  // Stop horizontal movement
                break;
            }
            break;
        }

        // Render frame if needed
        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            // Clear screen
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Draw game elements
            highway.draw();

            // Draw HUD
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0,
                "Score: %d", highway.getScore());
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 30, 0,
                "Level: %d", highway.getLevel());

            // Display coin count and coins needed for next level
            if (highway.getLevel() < Highway::MAX_LEVEL) {
                int coinsNeeded = Highway::COINS_FOR_LEVEL_UP * highway.getLevel() - highway.coinCollected;
                if (coinsNeeded < 0) coinsNeeded = 0;

                al_draw_textf(font, al_map_rgb(255, 215, 0), 10, 50, 0,
                    "Coins: %d/%d", highway.coinCollected, Highway::COINS_FOR_LEVEL_UP * highway.getLevel());
            }
            else {
                // At max level, just show coin count
                al_draw_textf(font, al_map_rgb(255, 215, 0), 10, 50, 0,
                    "Coins: %d", highway.coinCollected);
            }

            // Show controls hint
            al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH - 10, 10, ALLEGRO_ALIGN_RIGHT,
                "M - Toggle Music");

            // Show level up notification
            if (showLevelUpMessage) {
                al_draw_filled_rectangle(
                    SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 30,
                    SCREEN_WIDTH / 2 + 120, SCREEN_HEIGHT / 2 + 30,
                    al_map_rgba(0, 0, 0, 200)
                );
                al_draw_textf(font, al_map_rgb(255, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 10,
                    ALLEGRO_ALIGN_CENTER, "LEVEL UP!");
                al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 10,
                    ALLEGRO_ALIGN_CENTER, "Level %d", highway.getLevel());
            }

            // Draw pause message if game is paused
            if (paused) {
                al_draw_text(font, al_map_rgb(255, 255, 0), SCREEN_WIDTH / 2,
                    SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER,
                    "PAUSED - Press P to continue");
            }

            // Flip display
            al_flip_display();
        }
    }

    // Stop sound when game is over
    if (game_sound_instance) {
        al_stop_sample_instance(game_sound_instance);
    }

    // Game over screen
    if (highway.isGameOver) {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50,
            ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20,
            ALLEGRO_ALIGN_CENTER, "Final Score: %d", highway.getScore());
        al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 10,
            ALLEGRO_ALIGN_CENTER, "Level Reached: %d", highway.getLevel());
        al_draw_textf(font, al_map_rgb(255, 215, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 40,
            ALLEGRO_ALIGN_CENTER, "Coins Collected: %d", highway.coinCollected);
        al_flip_display();
        al_rest(3.0); // Show game over screen for 3 seconds
    }
}

void cleanup_game() {
    // Clean up game-specific resources
    if (font) {
        al_destroy_font(font);
    }

    // Clean up audio resources
    if (game_sound_instance) {
        al_destroy_sample_instance(game_sound_instance);
    }
    if (game_sound) {
        al_destroy_sample(game_sound);
    }
}

void cleanup_allegro() {
    // Clean up Allegro resources
    if (timer) {
        al_destroy_timer(timer);
    }
    if (event_queue) {
        al_destroy_event_queue(event_queue);
    }
    if (display) {
        al_destroy_display(display);
    }

    // Uninstall audio
    al_uninstall_audio();
}