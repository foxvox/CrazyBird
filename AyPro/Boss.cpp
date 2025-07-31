#include "Boss.hpp" 

static constexpr float HEALTH_BAR_OFFSET_Y = 100.f;   

/* 
void Boss::chooseRandomDirection() 
{ 
    // C++11���� ���Ե� ���� ���� Ŭ���� ���ø� �� �ϳ��Դϴ�. 
    std::uniform_real_distribution<float> dist(-1.f, 1.f);

    moveDir.x = dist(rng);
    moveDir.y = dist(rng);

    // ���̰� 0�� ������ ������
    if (std::abs(moveDir.x) < 0.1f && std::abs(moveDir.y) < 0.1f) {
        moveDir.x = 1.f;  // ���� fallback
    }

    // ����ȭ
    float len = std::sqrt(moveDir.x * moveDir.x + moveDir.y * moveDir.y);
    moveDir /= len;

    // ���ӽð��� 1~3�� ���� ����
    moveDuration = std::uniform_real_distribution<float>(1.f, 3.f)(rng);
    moveTimer = 0.f;
} 
*/ 

void Boss::chooseRandomDirectionTowardsPlayer(const sf::Vector2f& playerPos)
{
    // 1) ���� ���� ���� �� ����ȭ
    std::uniform_real_distribution<float> dist(-1.f, 1.f);
    sf::Vector2f randomDir{ dist(rng), dist(rng) };
    float lenR = std::sqrt(randomDir.x * randomDir.x + randomDir.y * randomDir.y);
    randomDir /= (lenR > 0 ? lenR : 1.f);

    // 2) �÷��̾� ���� ��� �� ����ȭ
    sf::Vector2f toPlayer = playerPos - s.getPosition();
    float lenP = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
    toPlayer /= (lenP > 0 ? lenP : 1.f);

    // 3) 20% Ȯ���� �÷��̾� ����
    std::bernoulli_distribution choosePlayer(0.2f);
    if (choosePlayer(rng)) 
    {
        moveDir = toPlayer;
    }
    else 
    {
        moveDir = randomDir;
    }

    // 4) �̵� ���ӽð�(1~3��)�� Ÿ�̸� �ʱ�ȭ
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

	moveSpeed = 100.f;          // ���� �̵� �ӵ�
	moveTimer = 0.f;            // �̵� �ð� �ʱ�ȭ 
    moveDuration = 2.f;         // �̵� ���� ���� �ֱ�

    chooseRandomDirectionTowardsPlayer(player.getPosition());  
}

void Boss::initHealthBar(ResourceManager& rm)
{
    barBack.setTexture(rm.get("bar1.png"));
    barFill.setTexture(rm.get("bar2.png"));
    // �ʱ� ��ġ�� updateHealthBar()���� �Ź� �缳��
}

void Boss::updateHealthBar()
{
    // 1) ü�� ���� ���
    float hpRatio = float(BOSS_HIT_REQUIRED - hits) / BOSS_HIT_REQUIRED; 
    // std::clamp�� �־��� ���� �ּҰ�(min)�� �ִ밪(max) ���̿� �ֵ��� �����ϴ� ��ƿ��Ƽ �Լ��Դϴ�.
    hpRatio = std::clamp(hpRatio, 0.f, 1.f);

    // 2) barFill �ʺ� ����
    auto texSize = barFill.getTexture()->getSize();
    barFill.setTextureRect({
        0, 0,
        static_cast<int>(texSize.x * hpRatio),
        static_cast<int>(texSize.y)
        }); 

    // barBack/barFill ��� ���� ��ǥ
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
    cleared = true;         // �� �� Ŭ���� ó��
    hits = 0;
	hasEntered = false;     // �ٽ� ���� �����ϵ��� �ʱ�ȭ 
    bullets.clear();        // ȭ�鿡 ���� �Ѿ� ���� ����
}

void Boss::update(float dt, ResourceManager& rm)
{
    if (!active) return;

    // ���� ����: ȭ�� �������� ������ 
    if (!hasEntered) 
    {
        constexpr float ENTER_SPEED = 200.f;
        float targetX = WINDOW_WIDTH - s.getGlobalBounds().width / 2 - 50.f;

        // ��ǥ ������ ���� ������ ��� �������� �̵�
        if (s.getPosition().x > targetX) 
        {
            s.move(-ENTER_SPEED * dt, 0.f);
            updateHealthBar();
            return;  // ���� �Ϸ� ������ �Ѿ�/���� �̵� ���� 
        }

        hasEntered = true;  // �� ���� ���� �� ���� �����Ӻ��� ���� ���� ����
		active = true;      // ���� �Ϸ� �� Ȱ��ȭ 
        chooseRandomDirectionTowardsPlayer(player.getPosition());
    }
        
	// Boss �Ѿ� ȸ���� ������� �߻� ó�� �κ�, 0.8�ʸ��� 
    if (shootClock.getElapsedTime().asSeconds() > 0.8f)
    {
        shootClock.restart();

        // �ݺ� �߻縶�� ������ ȸ���ϵ��� ���� ���� 
        static float rotationAngle = 0.f; 
        // ȸ�� �ӵ� : 1�ʿ� 2���� ȸ�� 
        constexpr float ROTATION_SPEED = PI * 4;      
        rotationAngle += ROTATION_SPEED * dt;

        // �Ѿ� �Ӽ�
        constexpr float RADIAL_SPEED = 50.f;
        constexpr float SWIRL_SPEED = 70.f;

        sf::Vector2f bp = s.getPosition();

        for (int i = 0; i < 8; ++i)
        {
            // 8���� �⺻ ���� + ���� ȸ��
            float ang = rotationAngle + i * (2 * PI / 8);

            // ���� ���� ����
            sf::Vector2f dir{ std::cos(ang), std::sin(ang) };

            // ����� ���� + ���� ����(�� �κ��� ��մ� �κ�: y���� ��ȣ�� �ٲ㼭 x�� �ְ� x���� y�� ������ �ݽð� ���� 90�� ȸ���� �� ���� ���� ������ �ȴ�.)
            sf::Vector2f vel = dir * RADIAL_SPEED + sf::Vector2f(-dir.y, dir.x) * SWIRL_SPEED; 

            bullets.emplace_back(rm, "boss_bullet.png", bp, vel, 1, 0.f);
        }
    }

    // �߰�: ȭ�� �� ���� �̵�
    moveTimer += dt;
    if (moveTimer >= moveDuration) 
    {
        chooseRandomDirectionTowardsPlayer(player.getPosition()); 
		moveTimer = 0.f;  // Ÿ�̸� �ʱ�ȭ 
    }

    // ���� �̵�
    s.move(moveDir * moveSpeed * dt);

    // ȭ�� ��� ������ Ŭ����
    auto pos = s.getPosition();
    auto halfWidth = s.getGlobalBounds().width / 2.f; 
	auto halfHeight = s.getGlobalBounds().height / 2.f + 40.f;  
	// x��ǥ�� ���� ���(WINDOW_WIDTH / 3)�� ������ ���(WINDOW_WIDTH - halfWidth) ���̷� ���� 
    pos.x = std::clamp(pos.x, WINDOW_WIDTH / 3.f, WINDOW_WIDTH - halfWidth); 
	// y��ǥ�� ��� ���(halfHeight)�� �ϴ� ���(WINDOW_HEIGHT - halfHeight) ���̷� ���� 
    pos.y = std::clamp(pos.y, halfHeight, WINDOW_HEIGHT - halfHeight);
    s.setPosition(pos);
    
    anim.update(dt); 
    updateHealthBar();
}

void Boss::draw(sf::RenderWindow& window)
{
    if (active && !cleared)
    {
        window.draw(s);         // ����
        window.draw(barBack);   // ü�¹� ���
        window.draw(barFill);   // ä���� ü�¹�
    }
} 


