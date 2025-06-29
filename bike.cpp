#include "bike.h"
#include "game.h"
#include <iostream>

Bike::Bike() : image(nullptr), width(100), height(110) {  // Increased to 50x85
    loadImage();
}

Bike::~Bike() {
    if (image) al_destroy_bitmap(image);
}

void Bike::loadImage() {
    image = al_load_bitmap("assets/bike.png");
    if (!image) {
        std::cerr << "Failed to load bike image\n";
        image = al_create_bitmap(width, height);
        al_set_target_bitmap(image);
        al_clear_to_color(al_map_rgb(0, 255, 0));
        al_set_target_backbuffer(al_get_current_display());
    }
    else {
        // Scale to desired size while maintaining aspect ratio
        float scale = std::min((float)width / al_get_bitmap_width(image),
            (float)height / al_get_bitmap_height(image));
        int scaled_w = al_get_bitmap_width(image) * scale;
        int scaled_h = al_get_bitmap_height(image) * scale;

        ALLEGRO_BITMAP* scaled = al_create_bitmap(width, height);
        al_set_target_bitmap(scaled);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0)); // Transparent background
        al_draw_scaled_bitmap(image,
            0, 0, al_get_bitmap_width(image), al_get_bitmap_height(image),
            (width - scaled_w) / 2, (height - scaled_h) / 2, scaled_w, scaled_h, 0);
        al_destroy_bitmap(image);
        image = scaled;
        al_set_target_backbuffer(al_get_current_display());
    }
}

void Bike::draw(float x, float y) {
    if (image) {
        al_draw_bitmap(image, x, y, 0);
    }
    else {
        al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(0, 255, 0));
    }