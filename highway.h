#ifndef HIGHWAY_H
#define HIGHWAY_H

#include <vector>
#include <memory>
#include <allegro5/allegro.h>

// Forward declarations
class Bike;
class Player;
class Coin;

// Base class for all game objects
class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void update() = 0;
    virtual void draw() = 0;
};

// Lane configuration constants
constexpr int LANE_COUNT = 3;
constexpr int LANE_WIDTH = 200;
constexpr int FIRST_LANE_X = 100;
constexpr int LANE_POSITIONS[LANE_COUNT] = {
    FIRST_LANE_X,
    FIRST_LANE_X + LANE_WIDTH,
    FIRST_LANE_X + 2 * LANE_WIDTH
};

class Obstacle : public GameObject {
public:
    static const int WIDTH = 80;
    static const int HEIGHT = 120;
    static const char* CAR_IMAGES[3];

    float x, y;
    float speed;
    ALLEGRO_BITMAP* image;

    // Collision box padding
    static constexpr float collisionOffsetX = 15.0f;
    static constexpr float collisionOffsetY = 20.0f;

    Obstacle(float start_x, float start_y, float speed);
    ~Obstacle();

    void update() override;
    void draw() override;
    bool checkCollision(const Player& player) const;

    float getCollisionX() const { return x + collisionOffsetX; }
    float getCollisionY() const { return y + collisionOffsetY; }
    float getCollisionWidth() const { return WIDTH - 2 * collisionOffsetX; }
    float getCollisionHeight() const { return HEIGHT - 2 * collisionOffsetY; }
};

class Highway {
public:
    bool isGameOver;
    int score;
    int coinCollected;  // Track number of coins collected
    int currentLevel;   // Track current level
    static const int MAX_LEVEL = 3; // Maximum level
    static const int COINS_FOR_LEVEL_UP = 12; // Coins needed to level up

    Highway(Bike& bike, Player& player);
    void update();
    void draw();
    void checkCollisions();
    int getScore() const { return score; }
    int getLevel() const { return currentLevel; }
    void increaseLevel(); // Method to handle level-up

private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<std::unique_ptr<GameObject>> coins; // Separate coins collection
    ALLEGRO_BITMAP* background;
    Bike& playerBike;
    Player& player;
    float backgroundY;
    float baseSpeed; // Base speed for obstacles
    float scrollSpeed; // Scrolling speed for background

    void generateObstacles();
    void spawnCoins();
    void loadBackground();
    void checkLevelProgress(); // Check if we should level up
};

#endif // HIGHWAY_H