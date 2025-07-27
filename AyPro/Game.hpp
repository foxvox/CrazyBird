#pragma once
#include "Common.hpp" 
#include "ResourceManager.hpp"
#include "Background.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Coin.hpp"
#include "Pet.hpp"
#include "Boss.hpp"
#include "Player.hpp" 

class Game
{    
public:
    static int           coinCount;
	static float	     deltaTime;
    bool 			     gameOver = false;
    bool                 ultimate = false; 
    bool                 alarmFlag = true; 
    sf::Font             scoreFont;   // ��Ʈ
    sf::Text             scoreText;   // �ؽ�Ʈ

private:
    sf::RenderWindow     window;
    ResourceManager&     rm;
    Background           farBg, midBg, nearBg;
    sf::View             view;
    sf::Clock            spawnClock;
    std::vector<Bullet>  bullets;
    std::vector<Enemy>   enemies;
    std::vector<Coin>    coins;
    Player               player;
    Pet                  pet1, pet2, pet3, pet4, pet5, pet6;
    Boss                 boss;
    InputState           input;

    sf::RectangleShape   bossWarningOverlay{ sf::Vector2f(window.getSize()) };
    float                blinkTimer = 0.0f;
    bool                 blinkVisible = false;


private: // ī�޶� ��鸲 ���� ����
    float                shakeDuration = 0.f;        // ���� ��鸲 �ð�
    float                shakeMagnitude = 0.f;       // ��鸲 ����
    sf::Vector2f         originalViewCenter;         // ��鸲 �� ���� �� �߽� 

public: 
    Game(ResourceManager& r);
    void run(); 
    void triggerCameraShake(float duration, float magnitude); 
	void alarmBos(float dt);            // ���� ���� �� ����� �� �������� ó��
    void bgUpdate(float dt);            // ��� ������Ʈ
	void spawnEnemy();                  // �� ����   
	void bossUpdate(float dt);          // ���� ������Ʈ 
	void coinUpdate(float dt);          // ���� ������Ʈ 
    void petUpdate(float dt);
    void update(float dt, const InputState& input);
    void draw(); 

    void handleCollisions(); 
	void eraseInactiveObjects();
}; 
