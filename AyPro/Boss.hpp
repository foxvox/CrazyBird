#pragma once 
#include "Player.hpp" 
#include <random> 

class Boss
{
public:
    sf::Sprite          s;
    bool                active = false;
    bool                cleared = false;
    int                 hits = 0;
    Animation           anim;
    std::vector<Bullet> bullets;
    sf::Clock           shootClock;

    sf::Sprite          barBack;      // 체력바 배경
    sf::Sprite          barFill;      // 채워진 체력바

	Player&             player;       // 플레이어 참조

    // 호출 순서:
    //  Game::Game() 에서 initHealthBar()
    //  매 프레임 updateHealthBar(), draw() 호출
    void initHealthBar(ResourceManager& rm);
    void updateHealthBar();
    void draw(sf::RenderWindow& window);

    Boss(ResourceManager& rm, Player& _player); 
    void start();
    void clear();
    void update(float dt, ResourceManager& rm);

private:
    // 엔진 선언 및 시드 초기화 Random Number Generator의 약자 
    std::mt19937 rng{ std::random_device{}() }; 

	bool hasEntered = false;           // 화면 안쪽으로 진입 완료 여부    
    // 위치 이동 관련
    sf::Vector2f moveDir;      // 현재 이동 방향 (단위 벡터)
    float moveTimer;           // 현재 방향으로 이동한 시간 누적
    float moveDuration;        // 이 방향으로 이동할 총 시간
    float moveSpeed;           // 이동 속도

    void chooseRandomDirection();  // 새 방향과 시간을 결정 
    void chooseRandomDirectionTowardsPlayer(const sf::Vector2f& playerPos);
}; 

