#include "Game.hpp" 
#include "SoundManager.hpp"
#include "SoundEffectManager.hpp" 

SoundManager        sm;
SoundEffectManager  sem;

int   Game::coinCount = 0;  
float Game::deltaTime = 0.f;

inline static bool checkCollision(const sf::Sprite& a, const sf::Sprite& b)
{
    return a.getGlobalBounds().intersects(b.getGlobalBounds());
} 

Game::Game(ResourceManager& r) 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Shooting Bird")
    , rm(r)
    , farBg(r, "bg1.png", 50)
    , midBg(r, "bg2.png", 150)
    , nearBg(r, "bg3.png", 300)
    , view(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT))
    , player(r, "player.png", 4)
    , pet1(r, "pet.png", 8, -20.f, -50.f)
    , pet2(r, "pet.png", 8, -20.f, 50.f)
    , pet3(r, "pet.png", 8, 0.f, -100.f)
    , pet4(r, "pet.png", 8, 0.f, 100.f)
    , pet5(r, "pet.png", 8, -20.f, -150.f)
    , pet6(r, "pet.png", 8, -20.f, 150.f)
    , boss(r, player)
{
    window.setFramerateLimit(60);
    view.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    bullets.reserve(512);
    enemies.reserve(512);
    coins.reserve(512); 

    sm.load("boss", "boss_sound.ogg");    
    sem.load("ultimate", "ultimate_sound.wav");
    sem.load("shield", "shield_sound.wav");
    sem.load("alarm", "alarm_sound.wav");
    sem.load("congrats", "congrats_sound.wav");

    r.get("bar1.png");
    r.get("bar2.png");
	boss.initHealthBar(r); 

    // ��Ʈ �ε�
    if (!scoreFont.loadFromFile("NanumBarunGothicBold.ttf")) 
    {
        throw std::runtime_error("Failed to load font NanumBarunGothicBold.ttf");
    }

    // �ؽ�Ʈ �⺻ �Ӽ� ����
    scoreText.setFont(scoreFont); 
    scoreText.setCharacterSize(40);                  // ũ�� ����
    scoreText.setFillColor(sf::Color::White);        // ����
    scoreText.setStyle(sf::Text::Bold);

    // ���� ��ܿ� ����
    const float margin = 25.f;                       // ȭ�� �׵θ� ����
    scoreText.setString("Score: 0");                 // �ʱ� ���ڿ�    
    sf::FloatRect rect = scoreText.getLocalBounds();
    scoreText.setOrigin(rect.width, 0.f);
    scoreText.setPosition(WINDOW_WIDTH / 2.f, margin); 
}

void Game::run()
{
    sf::Clock clock; 	
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        sf::Event e; 

        if (dt < 0.1f)
        {
            deltaTime = dt; 
        }
        
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();

            // Ű ������ �� ���� ó�� 
            if (e.type == sf::Event::KeyPressed)
            {
                switch (e.key.code)
                {
                case sf::Keyboard::F: 
                    player.activateInvincibility();
                    sem.playAsync("shield");           
                    break;

                case sf::Keyboard::R:
                    pet1.activateUltimate();
                    sem.playAsync("ultimate");                    
                    break;

                default:
                    break;
                }
            }
        }

		// ������ �ִ� ���ȿ��� ��� true ��ȯ ���� ���� false ��ȯ 
        input.up = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        input.down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        input.left = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        input.right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

        if (dt < 0.1f)
        {
            update(dt, input);
            draw();
        }
    }
}  

void Game::triggerCameraShake(float duration, float magnitude)
{
    shakeDuration = duration;
    shakeMagnitude = magnitude;
    originalViewCenter = view.getCenter();
}

void Game::handleCollisions()
{
    // ���� �����϶� ��ȣ���� �浹 ó��
    if (player.isInvincible())
    {
        const auto& shield = player.getShieldSprite();
        // ���� �Ѿ� & ��ȣ�� �浹 ó��
        for (auto& bb : boss.bullets)
        {
            if (bb.alive && checkCollision(bb.s, shield))
                bb.alive = false;
        }
        // �� & ��ȣ�� �浹 ó��
        for (auto& e : enemies)
        {
            if (e.alive && checkCollision(e.s, shield))
            {
                e.alive = false;
                coins.emplace_back(rm, e.s.getPosition());
            }
        }
    }

    // �浹 ó��: �� & �Ѿ�, �� & �÷��̾�
    for (auto& e : enemies)
    {
        for (auto& b : bullets)
        {
            if (b.alive && e.alive && checkCollision(b.s, e.s))
            {
                b.alive = false;
                e.alive = false;
                coins.emplace_back(rm, e.s.getPosition());
            }
        }

        if (e.alive && checkCollision(e.s, player.s))
        {
            e.alive = false;
            player.hit();
        }
    }

    // ���� ���°� �ƴ� �� ���� �Ѿ˰� �÷��̾� �浹 ó��
    for (auto& bb : boss.bullets)
    {
        if (bb.alive && checkCollision(bb.s, player.s)) // boss_bullet & player �浹 
        {
            bb.alive = false;
            player.hit();
        }
    }

    // �Ѿ˰� ���� �浹 ó��
    for (auto& b : bullets)
    {
        if (!boss.active && boss.cleared)
            break;

		// �÷��̾� �Ѿ˰� ���� �浹 ó��
        if (b.alive && checkCollision(b.s, boss.s))
        {
            b.alive = false;
            boss.hits++; 
			coinCount += 2; // �������� �Ѿ��� ���� ������ ���� 2�� �߰� �� ���� �߰� 

            // 100 ��Ʈ���� ī�޶� ��鸲
            if (boss.hits % 100 == 0)
                triggerCameraShake(0.5f, 5.f);
        }

        if (boss.hits >= BOSS_HIT_REQUIRED)
        {
            // Boss Ŭ������ ���ǵ� clear() ���
            boss.clear();

            // ���� óġ ����
            for (int i = 0; i < 50; ++i) 
            {
                sf::Vector2f pos = boss.s.getPosition() + sf::Vector2f(std::rand() % 100 - 100, std::rand() % 100 - 100);
                coins.emplace_back(rm, pos);
            }
			
			sem.playAsync("congrats"); // ���� óġ ���� ���� ȿ�� ���
            break;
        }
    }

} 

// ��Ȱ��ȭ�� emeies, bullets, boss.bullets ���� 
void Game::eraseInactiveObjects()
{
    auto predicate = [](auto& obj) { return !obj.alive; };
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), predicate), enemies.end());
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), predicate), bullets.end());
    boss.bullets.erase(std::remove_if(boss.bullets.begin(), boss.bullets.end(), predicate), boss.bullets.end()); 
}

void Game::bgUpdate(float dt)
{
    farBg.update(dt);
    midBg.update(dt);
    nearBg.update(dt);
} 

void Game::spawnEnemy()
{
    if (!boss.active && !boss.cleared && spawnClock.getElapsedTime().asSeconds() > ENEMY_SPAWN_INTERVAL)
    {
        spawnClock.restart();
        
        for (int i = 0; i < SIMULTANEOUS_SPAWN_COUNT; ++i)
        {
            if (std::rand() % 2)
                enemies.emplace_back(rm, "enemy1.png", 2, 30.f); // emplace_back�� ���ڴ� Enemy �����ڿ� ����� �� 
            else
                enemies.emplace_back(rm, "enemy2.png", 6, 30.f);
        }
    }
} 

void Game::bossUpdate(float dt)
{
	// ������ Ȱ��ȭ���� �ʾҰ� Ŭ������� ���� ��쿡�� ���� ������Ʈ. 
    if (!boss.active && !boss.cleared && coinCount > COINS_FOR_BOSS_ACTIVE)
    {
        sm.play("boss");
        boss.start();
        boss.update(dt, rm);
        boss.updateHealthBar();
    }
    else if (boss.active)
    {
        boss.update(dt, rm);
        boss.updateHealthBar();
                
        if (boss.s.getPosition().x < WINDOW_WIDTH - 200.f)
        {
            boss.s.setPosition(WINDOW_WIDTH - 200.f, boss.s.getPosition().y);
        }
    }
} 

void Game::coinUpdate(float dt)
{
    for (auto it = coins.begin(); it != coins.end();)
    {
        it->update(dt, player.s.getPosition());
        if (it->collected)
        {
            float dx = player.s.getPosition().x - it->s.getPosition().x;
            float dy = player.s.getPosition().y - it->s.getPosition().y;
            if (std::hypot(dx, dy) <= 5.f)
            {
                ++coinCount;
                it = coins.erase(it);
                continue;
            }
        }

        // ��� ���� �ƴϰ� ȭ�� ������ ���� ���ε� ����
        if (!it->collected && it->s.getPosition().y > WINDOW_HEIGHT + 50)
        {
            it = coins.erase(it);
        }
        else
        {
            ++it;
        }
    }
} 

void Game::petUpdate(float dt)
{
    ultimate = pet1.isUltimate();

    if (coinCount >= COINS_FOR_PET1)
    {
        pet1.active = true;
        pet1.update(dt, player.s.getPosition(), rm, bullets);
    }
    if (coinCount >= COINS_FOR_PET2)
    {
        pet2.active = true;
        pet2.update(dt, player.s.getPosition(), rm, bullets);
    }

    if (ultimate)
    {
        pet3.active = true;
        pet4.active = true;
        pet5.active = true;
        pet6.active = true;

        pet3.update(dt, player.s.getPosition(), rm, bullets);
        pet4.update(dt, player.s.getPosition(), rm, bullets);
        pet5.update(dt, player.s.getPosition(), rm, bullets);
        pet6.update(dt, player.s.getPosition(), rm, bullets);
    }
    else
    {
        pet3.active = false;
        pet4.active = false;
        pet5.active = false;
        pet6.active = false;
    }
} 

void Game::alarmBos(float dt)
{
    // ���� ���� ���� ����� �︮�� 
    if (coinCount > COINS_FOR_BOSS_ACTIVE - 100 && alarmFlag)
    {
        for (int i = 0; i < 3; i++)
        {
            sem.playAsync("alarm");
        }

        alarmFlag = false; // �˶��� 3���� �︮���� ����
    }

    // ���� ��� �������� ������ ó�� 
    if (coinCount > COINS_FOR_BOSS_ACTIVE - 100 && !boss.active && !boss.cleared)
    {
        blinkTimer += deltaTime; // deltaTime�� ������ �� �ð� ����

        if (blinkTimer > 0.2f) // 0.2�� �������� ����
        {
            blinkVisible = !blinkVisible;
            blinkTimer = 0.0f;
        }

        if (blinkVisible)
        {
            bossWarningOverlay.setFillColor(sf::Color(255, 0, 0, 80)); // ������ ������
        }
        else
        {
            bossWarningOverlay.setFillColor(sf::Color(255, 0, 0, 0)); // ���� ����
        }
    }
    else
    {
        bossWarningOverlay.setFillColor(sf::Color(255, 0, 0, 0)); // ������ �� ������ ��Ȱ��ȭ
    }
} 

void Game::update(float dt, const InputState& input) 
{
	bgUpdate(dt);                           // ��� ������Ʈ     
	player.update(dt, rm, input, bullets);  // �÷��̾� ������Ʈ 
	spawnEnemy();                           // �� ���� 

	// ������ Ȱ��ȭ���� �ʾҰ� Ŭ������� ���� ��쿡�� �� ������Ʈ 
    if (!boss.active && !boss.cleared) 
    {
        for (auto& e : enemies) e.update(dt);
    }   

	alarmBos(dt);   // ���� ���� �� ����� �� �������� ó�� 

	bossUpdate(dt); // ���� ������Ʈ 

    // ���� �Ѿ� ������Ʈ 
    for (auto& bb : boss.bullets) bb.update(dt); 

    // ������ Ŭ����� ��� ���� ��� ������ ��� 
    if (boss.cleared) sm.play("bg"); 

    // �÷��̾� �Ѿ� ������Ʈ 
    for (auto& b : bullets) b.update(dt);

    // ���� �� ��� �浹 ó�� 
	handleCollisions(); 

	// ��Ȱ��ȭ�� ��, �Ѿ�, ���� �Ѿ� ���� 
    eraseInactiveObjects(); 

    // ���� ������Ʈ
	coinUpdate(dt); 

	// �� ������Ʈ 
    petUpdate(dt); 

    // ���� �� ��� ���� ���
    int score = coinCount * 100;
    scoreText.setString("Score: " + std::to_string(score));

	sem.cleanup(); // ȿ���� ���� 

    // ī�޶� ��鸲 ó��
    if (shakeDuration > 0.f)
    {
        shakeDuration -= dt;
        // ���� ������ ����
        float offsetX = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * shakeMagnitude;
        float offsetY = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * shakeMagnitude;
        view.setCenter(originalViewCenter + sf::Vector2f(offsetX, offsetY));

        // ��鸲 ���� �� ����ġ ����
        if (shakeDuration <= 0.f)
            view.setCenter(originalViewCenter);
    }
}

void Game::draw()
{
    window.clear();
    window.setView(view);

    farBg.draw(window);
    midBg.draw(window);
    nearBg.draw(window); 

    window.draw(scoreText);

    // ������ Ȱ��ȭ���� �ʾҰ� Ŭ������� ���� ��쿡�� �� �׸��� 
    if (!boss.active && !boss.cleared)
        for (auto& e : enemies) window.draw(e.s);

    // �Ѿ� �׸��� 
    for (auto& b : bullets) window.draw(b.s);

    // ������ Ȱ��ȭ�ǰ� Ŭ������� ���� ��쿡�� ���� �Ѿ� �׸��� 
    if (boss.active && !boss.cleared)
        for (auto& bb : boss.bullets) window.draw(bb.s);

    // ���� �׸��� 
    for (auto& c : coins) window.draw(c.s);

    // ������ Ȱ��ȭ�ǰ� Ŭ������� ���� ��쿡�� ���� �׸���
    if (boss.active && !boss.cleared)
    {
		boss.draw(window); 
    }        

    // �÷��̾� �׸��� 
    player.draw(window);

    // �� �׸��� 
    if (coinCount >= COINS_FOR_PET1)
    {
        window.draw(pet1.s);
		
    }

    if (coinCount >= COINS_FOR_PET2)
    {
        window.draw(pet2.s); 
		pet2.active = true; 
    }

	if (ultimate) 
    {
        pet3.draw(window);
        pet4.draw(window);
        pet5.draw(window);
        pet6.draw(window);
	}

    window.draw(bossWarningOverlay);
    window.display();    
}