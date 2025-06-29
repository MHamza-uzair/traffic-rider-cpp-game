#ifndef COIN_H
#define COIN_H

#include "highway.h" // Contains GameObject definition
#include <allegro5/allegro.h>

class Player; // Forward declaration

class Coin : public GameObject {
public:
    static constexpr int WIDTH = 20;
    static constexpr int HEIGHT = 20;
    static constexpr int COLLISION_PADDING = 5;

    Coin(float x, float y);
    ~Coin();

    void update() override;
    void draw() override;
    bool checkCollision(const Player& player) const;

    float getX() const { return x; }
    float getY() const { return y; }
    bool isCollected() const { return collected; }
    void collect() { collected = true; }

    // New methods for level-based speed control
    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }

private:
    float x, y;
    float speed = 2.0f; // Default coin speed
    ALLEGRO_BITMAP* image;
    bool collected = false;

    void loadImage();
};

#endif // COIN_H