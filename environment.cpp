#include "environment.h"
#include "game.h"

Player::Player(float start_x, float start_y, int screen_h)
    : x(start_x), y(start_y), screen_height(screen_h),
    velocityX(0), velocityY(0), frameWidth(100), frameHeight(110) {
}

void Player::update() {
    x += velocityX;
    y += velocityY;

    // Boundary checking
    if (x < 100) x = 100;
    if (x > SCREEN_WIDTH - 100 - frameWidth) x = SCREEN_WIDTH - 100 - frameWidth;
    if (y < 0) y = 0;
    if (y > screen_height - frameHeight) y = screen_height - frameHeight;
}