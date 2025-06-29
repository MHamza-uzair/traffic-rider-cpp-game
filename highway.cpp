#include "highway.h"
#include "game.h"
#include "bike.h"
#include "environment.h"
#include "coin.h"
#include <allegro5/allegro_primitives.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

const char* Obstacle::CAR_IMAGES[3] = {
    "assets/car1.png",
    "assets/car2.png",
    "assets/car3.png"
};

Obstacle::Obstacle(float start_x, float start_y, float speed)
    : x(start_x), y(start_y), speed(speed), image(nullptr) {
    int carType = rand() % 3;
    image = al_load_bitmap(CAR_IMAGES[carType]);

    if (!image) {
        std::cerr << "Failed to load car image: " << CAR_IMAGES[carType] << "\n";
        image = al_create_bitmap(WIDTH, HEIGHT);
        al_set_target_bitmap(image);
        al_clear_to_color(al_map_rgb(255, 0, 0));
        al_set_target_backbuffer(al_get_current_display());
    }
}

Obstacle::~Obstacle() {
    if (image) al_destroy_bitmap(image);
}

void Obstacle::update() {
    y += speed;
    if (y > SCREEN_HEIGHT) {
        y = -HEIGHT;
        x = LANE_POSITIONS[rand() % LANE_COUNT] + (LANE_WIDTH - WIDTH) / 2;
        speed += 0.05f;

        int carType = rand() % 3;
        ALLEGRO_BITMAP* new_img = al_load_bitmap(CAR_IMAGES[carType]);
        if (new_img) {
            if (image) al_destroy_bitmap(image);
            image = new_img;
        }
    }
}

void Obstacle::draw() {
    if (image) {
        al_draw_scaled_bitmap(image,
            0, 0, al_get_bitmap_width(image), al_get_bitmap_height(image),
            x, y, WIDTH, HEIGHT, 0);
    }
    else {
        al_draw_filled_rectangle(x, y, x + WIDTH, y + HEIGHT, al_map_rgb(255, 0, 0));
    }
}

bool Obstacle::checkCollision(const Player& player) const {
    // Obstacle collision box
    float obsLeft = getCollisionX();
    float obsRight = obsLeft + getCollisionWidth();
    float obsTop = getCollisionY();
    float obsBottom = obsTop + getCollisionHeight();

    // Player collision box
    float playerLeft = player.x + player.collisionOffsetX;
    float playerRight = playerLeft + player.frameWidth - 2 * player.collisionOffsetX;
    float playerTop = player.y + player.collisionOffsetY;
    float playerBottom = playerTop + player.frameHeight - 2 * player.collisionOffsetY;

    // Check for overlap
    return obsLeft < playerRight &&
        obsRight > playerLeft &&
        obsTop < playerBottom &&
        obsBottom > playerTop;
}

Highway::Highway(Bike& bike, Player& player)
    : playerBike(bike), player(player), score(0), coinCollected(0), isGameOver(false),
    background(nullptr), backgroundY(0), currentLevel(1), baseSpeed(3.0f), scrollSpeed(2.0f) {
    std::srand(static_cast<unsigned>(time(nullptr)));
    loadBackground();
    generateObstacles();
    spawnCoins();
}

void Highway::loadBackground() {
    if (background) al_destroy_bitmap(background);
    background = al_load_bitmap("assets/background.png");

    if (!background) {
        std::cerr << "Failed to load background - using fallback\n";
        background = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
        al_set_target_bitmap(background);
        al_clear_to_color(al_map_rgb(50, 50, 150));

        for (int i = 0; i <= LANE_COUNT; i++) {
            al_draw_line(FIRST_LANE_X + i * LANE_WIDTH, 0,
                FIRST_LANE_X + i * LANE_WIDTH, SCREEN_HEIGHT,
                al_map_rgb(255, 255, 255), 2);
        }

        al_set_target_backbuffer(al_get_current_display());
    }
    else {
        ALLEGRO_BITMAP* stretched = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
        al_set_target_bitmap(stretched);
        al_draw_scaled_bitmap(
            background,
            0, 0, al_get_bitmap_width(background), al_get_bitmap_height(background),
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            0
        );
        al_destroy_bitmap(background);
        background = stretched;
        al_set_target_backbuffer(al_get_current_display());
    }
}

void Highway::generateObstacles() {
    gameObjects.clear();
    int obstacleCount = 5;
    for (int i = 0; i < obstacleCount; i++) {
        int lane = rand() % LANE_COUNT;
        float x = LANE_POSITIONS[lane] + (LANE_WIDTH - Obstacle::WIDTH) / 2;
        float y = -Obstacle::HEIGHT - (rand() % SCREEN_HEIGHT);
        float speed = baseSpeed + (rand() % 3);
        gameObjects.emplace_back(std::make_unique<Obstacle>(x, y, speed));
    }
}

void Highway::spawnCoins() {
    coins.clear();
    int coinCount = 3;

    for (int i = 0; i < coinCount; i++) {
        int lane = rand() % LANE_COUNT;
        float x = LANE_POSITIONS[lane] + (LANE_WIDTH - Coin::WIDTH) / 2;
        float y = -Coin::HEIGHT - (rand() % SCREEN_HEIGHT);
        coins.emplace_back(std::make_unique<Coin>(x, y));
    }
}

void Highway::update() {
    backgroundY += scrollSpeed;
    if (backgroundY >= SCREEN_HEIGHT) {
        backgroundY = 0;
    }

    // Update obstacles
    for (auto& obj : gameObjects) {
        obj->update();
    }

    // Update coins
    for (auto& coin : coins) {
        coin->update();
    }

    score++;
}

void Highway::draw() {
    // Draw the background
    if (background) {
        // Draw the background twice for scrolling effect
        al_draw_bitmap(background, 0, backgroundY - SCREEN_HEIGHT, 0);
        al_draw_bitmap(background, 0, backgroundY, 0);
    }

    // Draw obstacles
    for (auto& obj : gameObjects) {
        obj->draw();
    }

    // Draw coins
    for (auto& coin : coins) {
        coin->draw();
    }

    // Draw player
    playerBike.draw(player.x, player.y);
}

void Highway::checkCollisions() {
    // Check for collision with obstacles
    for (auto& obj : gameObjects) {
        auto* obstacle = dynamic_cast<Obstacle*>(obj.get());
        if (obstacle && obstacle->checkCollision(player)) {
            isGameOver = true;
            return;
        }
    }

    // Check for collision with coins
    for (auto& coinObj : coins) {
        auto* coin = dynamic_cast<Coin*>(coinObj.get());
        if (coin && !coin->isCollected() && coin->checkCollision(player)) {
            coin->collect();
            coinCollected++;
            score += 100;  // Add 100 points for collecting a coin

            // Check if player has collected enough coins to level up
            checkLevelProgress();
        }
    }
}

void Highway::checkLevelProgress() {
    // Check if player has collected enough coins to level up
    if (coinCollected >= COINS_FOR_LEVEL_UP * currentLevel && currentLevel < MAX_LEVEL) {
        increaseLevel();
    }
}

void Highway::increaseLevel() {
    // Increase level
    currentLevel++;

    // Increase base speed by 25%
    baseSpeed *= 1.25f;
    scrollSpeed *= 1.25f;

    // Apply new speed to existing obstacles
    for (auto& obj : gameObjects) {
        auto* obstacle = dynamic_cast<Obstacle*>(obj.get());
        if (obstacle) {
            obstacle->speed *= 1.25f;
        }
    }

    // Apply new speed to coins as well
    for (auto& coinObj : coins) {
        auto* coin = dynamic_cast<Coin*>(coinObj.get());
        if (coin) {
            coin->setSpeed(coin->getSpeed() * 1.25f);
        }
    }

    // Add bonus points for leveling up
    score += 500 * currentLevel;
}