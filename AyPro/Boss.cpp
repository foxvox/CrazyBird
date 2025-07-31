#include "Boss.hpp" 

static constexpr float HEALTH_BAR_OFFSET_Y = 100.f;   

/* 
void Boss::chooseRandomDirection() 
{ 
    // C++11부터 도입된 랜덤 분포 클래스 템플릿 중 하나입니다. 
    std::uniform_real_distribution<float> dist(-1.f, 1.f);

    moveDir.x = dist(rng);
    moveDir.y = dist(rng);

    // 길이가 0에 가까우면 재추출
    if (std::abs(moveDir.x) < 0.1f && std::abs(moveDir.y) < 0.1f) {
        moveDir.x = 1.f;  // 안전 fallback
    }

    // 정규화
    float len = std::sqrt(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
    moveDir /= len;

    // 지속시간도 1~3초 사이 랜덤
    moveDuration = std::uniform_real_distribution<float>(1.f, 3.f)(rng);
    moveTimer = 0.f;
} 
*/ 

void Boss::chooseRandomDirectionTowardsPlayer(const sf::Vector2f& playerPos)
{
    // 1) 랜덤 방향 생성 및 정규화
    std::uniform_real_distribution<float> dist(-1.f, 1.f);
    sf::Vector2f randomDir{ dist(rng), dist(rng) };
    float lenR = std::sqrt(randomDir.x * randomDir.x + randomDir.y * randomDir.y);
    randomDir /= (lenR > 0 ? lenR : 1.f);

    // 2) 플레이어 방향 계산 및 정규화
    sf::Vector2f toPlayer = playerPos - s.getPosition();
    float lenP = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
    toPlayer /= (lenP > 0 ? lenP : 1.f);

    // 3) 20% 확률로 플레이어 방향
    std::bernoulli_distribution choosePlayer(0.2f);
    if (choosePlayer(rng)) 
    {
        moveDir = toPlayer;
    }
    else 
    {
        moveDir = randomDir;
    }

    // 4) 이동 지속시간(1~3초)과 타이머 초기화
    moveDuration = std::uniform_real_distribution<float>(1.f, 3.f)(rng);
    moveTimer = 0.f;
}

Boss::Boss(ResourceManager& rm, Player& _player) 
    : s(rm.get("boss.png")), anim(s, 1, 1.f), player(_player) 
{
    auto bounds = s.getGlobalBounds();
    s.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    s.setPosition(WINDOW_WIDTH + 100.f, WINDOW_HEIGHT / 2.f);
    bullets.reserve(512);
      
    initHealthBar(rm);

	moveSpeed = 100.f;          // 보스 이동 속도
	moveTimer = 0.f;            // 이동 시간 초기화 
    moveDuration = 2.f;         // 이동 방향 변경 주기

    chooseRandomDirectionTowardsPlayer(player.getPosition());  
}

void Boss::initHealthBar(ResourceManager& rm)
{
    barBack.setTexture(rm.get("bar1.png"));
    barFill.setTexture(rm.get("bar2.png"));
    // 초기 위치는 updateHealthBar()에서 매번 재설정
}

void Boss::updateHealthBar()
{
    // 1) 체력 비율 계산
    float hpRatio = float(BOSS_HIT_REQUIRED - hits) / BOSS_HIT_REQUIRED; 
    // std::clamp는 주어진 값이 최소값(min)과 최대값(max) 사이에 있도록 보장하는 유틸리티 함수입니다.
    hpRatio = std::clamp(hpRatio, 0.f, 1.f);

    // 2) barFill 너비 조정
    auto texSize = barFill.getTexture()->getSize();
    barFill.setTextureRect({
        0, 0,
        static_cast<int>(texSize.x * hpRatio),
        static_cast<int>(texSize.y)
        }); 

    // barBack/barFill 모두 같은 좌표
    float halfBackW = barBack.getTextureRect().width / 2.f;
    sf::Vector2f bossPos = s.getPosition();
    sf::Vector2f barPos{ bossPos.x - halfBackW, bossPos.y - HEALTH_BAR_OFFSET_Y };
    barBack.setPosition(barPos);
    barFill.setPosition(barPos);
}

void Boss::start()
{
    active = true;
    cleared = false;
    hits = 0;
    shootClock.restart();
}

void Boss::clear()
{
    active = false;
    cleared = true;         // 한 번 클리어 처리
    hits = 0;
	hasEntered = false;     // 다시 진입 가능하도록 초기화 
    bullets.clear();        // 화면에 남은 총알 전부 제거
}

void Boss::update(float dt, ResourceManager& rm)
{
    if (!active) return;

    // 진입 로직: 화면 안쪽으로 들어오기 
    if (!hasEntered) 
    {
        constexpr float ENTER_SPEED = 200.f;
        float targetX = WINDOW_WIDTH - s.getGlobalBounds().width / 2 - 50.f;

        // 목표 지점에 도달 전까지 계속 왼쪽으로 이동
        if (s.getPosition().x > targetX) 
        {
            s.move(-ENTER_SPEED * dt, 0.f);
            updateHealthBar();
            return;  // 진입 완료 전에는 총알/보스 이동 무시 
        }

        hasEntered = true;  // 한 번만 진입 후 다음 프레임부터 진입 로직 생략
		active = true;      // 진입 완료 후 활성화 
        chooseRandomDirectionTowardsPlayer(player.getPosition());
    }
        
	// Boss 총알 회오리 모양으로 발사 처리 부분, 0.8초마다 
    if (shootClock.getElapsedTime().asSeconds() > 0.8f)
    {
        shootClock.restart();

        // 반복 발사마다 패턴이 회전하도록 누적 각도 
        static float rotationAngle = 0.f; 
        // 회전 속도 : 1초에 2바퀴 회전 
        constexpr float ROTATION_SPEED = PI * 4;      
        rotationAngle += ROTATION_SPEED * dt;

        // 총알 속성
        constexpr float RADIAL_SPEED = 50.f;
        constexpr float SWIRL_SPEED = 70.f;

        sf::Vector2f bp = s.getPosition();

        for (int i = 0; i < 8; ++i)
        {
            // 8방향 기본 각도 + 누적 회전
            float ang = rotationAngle + i * (2 * PI / 8);

            // 방향 단위 벡터
            sf::Vector2f dir{ std::cos(ang), std::sin(ang) };

            // 방사형 성분 + 접선 성분(이 부분이 재밌는 부분: y값의 부호를 바꿔서 x에 넣고 x값을 y에 넣으면 반시계 방향 90도 회전값 즉 접선 벡터 성분이 된다.)
            sf::Vector2f vel = dir * RADIAL_SPEED + sf::Vector2f(-dir.y, dir.x) * SWIRL_SPEED; 

            bullets.emplace_back(rm, "boss_bullet.png", bp, vel, 1, 0.f);
        }
    }

    // 추가: 화면 내 자유 이동
    moveTimer += dt;
    if (moveTimer >= moveDuration) 
    {
        chooseRandomDirectionTowardsPlayer(player.getPosition()); 
		moveTimer = 0.f;  // 타이머 초기화 
    }

    // 실제 이동
    s.move(moveDir * moveSpeed * dt);

    // 화면 경계 안으로 클램프
    auto pos = s.getPosition();
    auto halfWidth = s.getGlobalBounds().width / 2.f; 
	auto halfHeight = s.getGlobalBounds().height / 2.f + 40.f;  
	// x좌표는 왼쪽 경계(WINDOW_WIDTH / 3)와 오른쪽 경계(WINDOW_WIDTH - halfWidth) 사이로 제한 
    pos.x = std::clamp(pos.x, WINDOW_WIDTH / 3.f, WINDOW_WIDTH - halfWidth); 
	// y좌표는 상단 경계(halfHeight)와 하단 경계(WINDOW_HEIGHT - halfHeight) 사이로 제한 
    pos.y = std::clamp(pos.y, halfHeight, WINDOW_HEIGHT - halfHeight);
    s.setPosition(pos);
    
    anim.update(dt); 
    updateHealthBar();
}

void Boss::draw(sf::RenderWindow& window)
{
    if (active && !cleared)
    {
        window.draw(s);         // 보스
        window.draw(barBack);   // 체력바 배경
        window.draw(barFill);   // 채워진 체력바
    }
} 


