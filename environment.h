#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

class Player {
public:
    float x, y;
    float velocityX, velocityY;
    int screen_height;
    int frameWidth, frameHeight;

    // Collision padding
    static constexpr float collisionOffsetX = 20.0f;
    static constexpr float collisionOffsetY = 30.0f;

    Player(float start_x, float start_y, int screen_h);
    void update();
};

#endif // ENVIRONMENT_H