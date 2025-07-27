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
    sf::Font             scoreFont;   // 폰트
    sf::Text             scoreText;   // 텍스트

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


private: // 카메라 흔들림 관련 변수
    float                shakeDuration = 0.f;        // 남은 흔들림 시간
    float                shakeMagnitude = 0.f;       // 흔들림 강도
    sf::Vector2f         originalViewCenter;         // 흔들림 전 원래 뷰 중심 

public: 
    Game(ResourceManager& r);
    void run(); 
    void triggerCameraShake(float duration, float magnitude); 
	void alarmBos(float dt);            // 보스 등장 전 경고음 및 오버레이 처리
    void bgUpdate(float dt);            // 배경 업데이트
	void spawnEnemy();                  // 적 생성   
	void bossUpdate(float dt);          // 보스 업데이트 
	void coinUpdate(float dt);          // 코인 업데이트 
    void petUpdate(float dt);
    void update(float dt, const InputState& input);
    void draw(); 

    void handleCollisions(); 
	void eraseInactiveObjects();
}; 
