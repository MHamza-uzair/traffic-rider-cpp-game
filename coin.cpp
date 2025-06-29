#include "coin.h"
#include "environment.h"  // For Player class
#include "game.h"         // For SCREEN_WIDTH/HEIGHT
#include <allegro5/allegro_primitives.h>
#include <iostream>

const char* COIN_IMAGE = "assets/coin.png";

// Constructor
Coin::Coin(float start_x, float start_y)
    : x(start_x), y(start_y), image(nullptr), collected(false), speed(2.0f) {
    loadImage();
}

// Destructor
Coin::~Coin() {
    if (image) {
        al_destroy_bitmap(image);
    }
}

void Coin::update() {
    // Scroll with the road (now uses speed variable instead of hard-coded value)
    y += speed;

    // Respawn when off-screen
    if (y > SCREEN_HEIGHT) {
        y = -HEIGHT;
        collected = false;
    }
}

void Coin::draw() {
    if (!collected && image) {
        // If using original bitmap, scale it down to the new size
        int bmp_width = al_get_bitmap_width(image);
        int bmp_height = al_get_bitmap_height(image);

        if (bmp_width != WIDTH || bmp_height != HEIGHT) {
            al_draw_scaled_bitmap(image,
                0, 0, bmp_width, bmp_height,
                x, y, WIDTH, HEIGHT, 0);
        }
        else {
            al_draw_bitmap(image, x, y, 0);
        }

        /* Debug collision box (uncomment if needed)
        al_draw_rectangle(
            x + COLLISION_PADDING, y + COLLISION_PADDING,
            x + WIDTH - COLLISION_PADDING, y + HEIGHT - COLLISION_PADDING,
            al_map_rgba(0, 255, 0, 128), 1
        );*/
    }
}

bool Coin::checkCollision(const Player& player) const {
    if (collected) return false;

    // Coin collision box
    float coinLeft = x + COLLISION_PADDING;
    float coinRight = coinLeft + (WIDTH - 2 * COLLISION_PADDING);
    float coinTop = y + COLLISION_PADDING;
    float coinBottom = coinTop + (HEIGHT - 2 * COLLISION_PADDING);

    // Player collision box
    float playerLeft = player.x + player.collisionOffsetX;
    float playerRight = playerLeft + player.frameWidth - 2 * player.collisionOffsetX;
    float playerTop = player.y + player.collisionOffsetY;
    float playerBottom = playerTop + player.frameHeight - 2 * player.collisionOffsetY;

    return coinLeft < playerRight &&
        coinRight > playerLeft &&
        coinTop < playerBottom &&
        coinBottom > playerTop;
}

void Coin::loadImage() {
    ALLEGRO_BITMAP* original = al_load_bitmap(COIN_IMAGE);

    if (!original) {
        std::cerr << "Failed to load coin image - using fallback\n";
        image = al_create_bitmap(WIDTH, HEIGHT);
        al_set_target_bitmap(image);
        al_clear_to_color(al_map_rgb(255, 215, 0)); // Gold color
        al_draw_filled_circle(WIDTH / 2, HEIGHT / 2, WIDTH / 2 - 2, al_map_rgb(255, 215, 0));
        al_draw_circle(WIDTH / 2, HEIGHT / 2, WIDTH / 2 - 2, al_map_rgb(200, 170, 0), 1);
        al_set_target_backbuffer(al_get_current_display());
    }
    else {
        // Create a properly scaled version
        image = al_create_bitmap(WIDTH, HEIGHT);
        al_set_target_bitmap(image);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0)); // Transparent background
        al_draw_scaled_bitmap(original,
            0, 0, al_get_bitmap_width(original), al_get_bitmap_height(original),
            0, 0, WIDTH, HEIGHT, 0);
        al_destroy_bitmap(original);
        al_set_target_backbuffer(al_get_current_display());
    }
}