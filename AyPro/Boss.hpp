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

    sf::Sprite          barBack;      // ü�¹� ���
    sf::Sprite          barFill;      // ä���� ü�¹�

	Player&             player;       // �÷��̾� ����

    // ȣ�� ����:
    //  Game::Game() ���� initHealthBar()
    //  �� ������ updateHealthBar(), draw() ȣ��
    void initHealthBar(ResourceManager& rm);
    void updateHealthBar();
    void draw(sf::RenderWindow& window);

    Boss(ResourceManager& rm, Player& _player); 
    void start();
    void clear();
    void update(float dt, ResourceManager& rm);

private:
    // ���� ���� �� �õ� �ʱ�ȭ Random Number Generator�� ���� 
    std::mt19937 rng{ std::random_device{}() }; 

	bool hasEntered = false;           // ȭ�� �������� ���� �Ϸ� ����    
    // ��ġ �̵� ����
    sf::Vector2f moveDir;      // ���� �̵� ���� (���� ����)
    float moveTimer;           // ���� �������� �̵��� �ð� ����
    float moveDuration;        // �� �������� �̵��� �� �ð�
    float moveSpeed;           // �̵� �ӵ�

    void chooseRandomDirection();  // �� ����� �ð��� ���� 
    void chooseRandomDirectionTowardsPlayer(const sf::Vector2f& playerPos);
}; 

