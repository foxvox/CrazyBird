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

    // 폰트 로드
    if (!scoreFont.loadFromFile("NanumBarunGothicBold.ttf")) 
    {
        throw std::runtime_error("Failed to load font NanumBarunGothicBold.ttf");
    }

    // 텍스트 기본 속성 설정
    scoreText.setFont(scoreFont); 
    scoreText.setCharacterSize(40);                  // 크기 조정
    scoreText.setFillColor(sf::Color::White);        // 색상
    scoreText.setStyle(sf::Text::Bold);

    // 우측 상단에 놓기
    const float margin = 25.f;                       // 화면 테두리 여백
    scoreText.setString("Score: 0");                 // 초기 문자열    
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

            // 키 눌림을 한 번만 처리 
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

		// 누르고 있는 동안에는 계속 true 반환 떼는 순간 false 반환 
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
    // 무적 상태일때 보호막과 충돌 처리
    if (player.isInvincible())
    {
        const auto& shield = player.getShieldSprite();
        // 보스 총알 & 보호막 충돌 처리
        for (auto& bb : boss.bullets)
        {
            if (bb.alive && checkCollision(bb.s, shield))
                bb.alive = false;
        }
        // 적 & 보호막 충돌 처리
        for (auto& e : enemies)
        {
            if (e.alive && checkCollision(e.s, shield))
            {
                e.alive = false;
                coins.emplace_back(rm, e.s.getPosition());
            }
        }
    }

    // 충돌 처리: 적 & 총알, 적 & 플레이어
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

    // 무적 상태가 아닐 때 보스 총알과 플레이어 충돌 처리
    for (auto& bb : boss.bullets)
    {
        if (bb.alive && checkCollision(bb.s, player.s)) // boss_bullet & player 충돌 
        {
            bb.alive = false;
            player.hit();
        }
    }

    // 총알과 보스 충돌 처리
    for (auto& b : bullets)
    {
        if (!boss.active && boss.cleared)
            break;

		// 플레이어 총알과 보스 충돌 처리
        if (b.alive && checkCollision(b.s, boss.s))
        {
            b.alive = false;
            boss.hits++; 
			coinCount += 2; // 보스에게 총알이 맞을 때마다 코인 2개 추가 즉 점수 추가 

            // 100 히트마다 카메라 흔들림
            if (boss.hits % 100 == 0)
                triggerCameraShake(0.5f, 5.f);
        }

        if (boss.hits >= BOSS_HIT_REQUIRED)
        {
            // Boss 클래스에 정의된 clear() 사용
            boss.clear();

            // 보스 처치 보상
            for (int i = 0; i < 50; ++i) 
            {
                sf::Vector2f pos = boss.s.getPosition() + sf::Vector2f(std::rand() % 100 - 100, std::rand() % 100 - 100);
                coins.emplace_back(rm, pos);
            }
			
			sem.playAsync("congrats"); // 보스 처치 축하 사운드 효과 재생
            break;
        }
    }

} 

// 비활성화된 emeies, bullets, boss.bullets 제거 
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
                enemies.emplace_back(rm, "enemy1.png", 2, 30.f); // emplace_back의 인자는 Enemy 생성자에 맞춰야 함 
            else
                enemies.emplace_back(rm, "enemy2.png", 6, 30.f);
        }
    }
} 

void Game::bossUpdate(float dt)
{
	// 보스가 활성화되지 않았고 클리어되지 않은 경우에만 보스 업데이트. 
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

        // 흡수 중이 아니고 화면 밖으로 나간 코인도 제거
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
    // 보스 등장 전에 경고음 울리기 
    if (coinCount > COINS_FOR_BOSS_ACTIVE - 100 && alarmFlag)
    {
        for (int i = 0; i < 3; i++)
        {
            sem.playAsync("alarm");
        }

        alarmFlag = false; // 알람은 3번만 울리도록 설정
    }

    // 보스 경고 오버레이 깜박임 처리 
    if (coinCount > COINS_FOR_BOSS_ACTIVE - 100 && !boss.active && !boss.cleared)
    {
        blinkTimer += deltaTime; // deltaTime은 프레임 간 시간 간격

        if (blinkTimer > 0.2f) // 0.2초 간격으로 깜박
        {
            blinkVisible = !blinkVisible;
            blinkTimer = 0.0f;
        }

        if (blinkVisible)
        {
            bossWarningOverlay.setFillColor(sf::Color(255, 0, 0, 80)); // 빨간색 반투명
        }
        else
        {
            bossWarningOverlay.setFillColor(sf::Color(255, 0, 0, 0)); // 완전 투명
        }
    }
    else
    {
        bossWarningOverlay.setFillColor(sf::Color(255, 0, 0, 0)); // 조건이 안 맞으면 비활성화
    }
} 

void Game::update(float dt, const InputState& input) 
{
	bgUpdate(dt);                           // 배경 업데이트     
	player.update(dt, rm, input, bullets);  // 플레이어 업데이트 
	spawnEnemy();                           // 적 생성 

	// 보스가 활성화되지 않았고 클리어되지 않은 경우에만 적 업데이트 
    if (!boss.active && !boss.cleared) 
    {
        for (auto& e : enemies) e.update(dt);
    }   

	alarmBos(dt);   // 보스 등장 전 경고음 및 오버레이 처리 

	bossUpdate(dt); // 보스 업데이트 

    // 보스 총알 업데이트 
    for (auto& bb : boss.bullets) bb.update(dt); 

    // 보스가 클리어된 경우 원래 배경 음악을 재생 
    if (boss.cleared) sm.play("bg"); 

    // 플레이어 총알 업데이트 
    for (auto& b : bullets) b.update(dt);

    // 게임 내 모든 충돌 처리 
	handleCollisions(); 

	// 비활성화된 적, 총알, 보스 총알 제거 
    eraseInactiveObjects(); 

    // 코인 업데이트
	coinUpdate(dt); 

	// 펫 업데이트 
    petUpdate(dt); 

    // 코인 수 기반 점수 계산
    int score = coinCount * 100;
    scoreText.setString("Score: " + std::to_string(score));

	sem.cleanup(); // 효과음 정리 

    // 카메라 흔들림 처리
    if (shakeDuration > 0.f)
    {
        shakeDuration -= dt;
        // 랜덤 오프셋 생성
        float offsetX = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * shakeMagnitude;
        float offsetY = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * shakeMagnitude;
        view.setCenter(originalViewCenter + sf::Vector2f(offsetX, offsetY));

        // 흔들림 종료 시 원위치 복구
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

    // 보스가 활성화되지 않았고 클리어되지 않은 경우에만 적 그리기 
    if (!boss.active && !boss.cleared)
        for (auto& e : enemies) window.draw(e.s);

    // 총알 그리기 
    for (auto& b : bullets) window.draw(b.s);

    // 보스가 활성화되고 클리어되지 않은 경우에만 보스 총알 그리기 
    if (boss.active && !boss.cleared)
        for (auto& bb : boss.bullets) window.draw(bb.s);

    // 코인 그리기 
    for (auto& c : coins) window.draw(c.s);

    // 보스가 활성화되고 클리어되지 않은 경우에만 보스 그리기
    if (boss.active && !boss.cleared)
    {
		boss.draw(window); 
    }        

    // 플레이어 그리기 
    player.draw(window);

    // 펫 그리기 
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