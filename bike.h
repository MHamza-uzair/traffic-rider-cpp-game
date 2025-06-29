#ifndef BIKE_H
#define BIKE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

class Bike {
public:
    Bike();
    ~Bike();

    void draw(float x, float y);
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    ALLEGRO_BITMAP* image;
    int width;
    int height;

    void loadImage();
};

#endif // BIKE_H