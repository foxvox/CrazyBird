#pragma once 
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp> 
#include <iostream> 
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <memory> 

#define LOG(msg) std::cout << "[" << __FUNCTION__ << "] " << msg << "\n"; 

struct InputState
{
    bool up = false;             // W
    bool down = false;           // S
    bool left = false;           // A
    bool right = false;          // D
    bool invincibleKey = false;  // F
    bool ultimateKey = false;    // Rm
};

using namespace std;
using namespace sf;

constexpr int   WINDOW_WIDTH = 1280;
constexpr int   WINDOW_HEIGHT = 720;
constexpr float PLAYER_SPEED = 300.f;
constexpr float BULLET_SPEED = 800.f;
constexpr float PET_BULLET_SPEED = 800.f;
constexpr float ENEMY_SPEED = 500.f;
constexpr float ENEMY_SPAWN_INTERVAL = 0.5f;
constexpr int   SIMULTANEOUS_SPAWN_COUNT = 6;
constexpr float COIN_BOUNCE_SPEED = 400.f;
constexpr float COIN_ATTRACT_RADIUS = 600.f;
constexpr float COIN_ATTRACT_SPEED = 600.f;
constexpr int   COINS_FOR_PET1 = 20;
constexpr int   COINS_FOR_PET2 = 40; 
constexpr int   COINS_FOR_ULTIMATE = 50; 
constexpr int   COINS_FOR_INVINCIBLE = 50; 
constexpr int   COINS_FOR_BOSS_ACTIVE = 500; 
constexpr float ULTIMATE_DURATION = 8.f; 
constexpr float INVINCIBLE_DURATION = 8.f;
constexpr int   BOSS_HIT_REQUIRED = 500;
constexpr float PI = 3.14159f; 
constexpr float HIT_DURATION = 2.f; 
