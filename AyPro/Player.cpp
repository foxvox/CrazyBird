#include "Player.hpp" 
#include "Game.hpp" 

void Player::hitEffect(float dt) 
{
    // 플레이어가 맞았을 때 깜박이는 효과  
    if (hitTimer > 0.f)
    {
        hitTimer -= dt;

        if (hitTimer <= 0.f)
        {
            hitTimer = 0.f;
            hitSprite.setColor(sf::Color::White);
            return;
        }

        // hit 애니메이션 업데이트 하고 나서 깜박이는 효과 적용 
        hitAnim->update(dt);
        // blink 효과           
        float alpha = (std::sin(hitTimer * 20) + 1.f) * 0.5f * 255;
        hitSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
    }
}

void Player::invinEffect(float dt)
{
    // 무적 상태일 때 깜박이는 효과 
    if (invincible)
    {
        invTimer -= dt;
        if (invTimer <= 0.f)
        {
			invTimer = 0.f; 
            invincible = false;
            s.setColor(sf::Color::White);
            return;
        }
        // blink 효과 
        float alpha = (std::sin(invTimer * 20) + 1.f) * 0.5f * 255;
        shieldSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
    }
}

void Player::activateInvincibility()
{
    if (!invincible && Game::coinCount >= COINS_FOR_INVINCIBLE)
    {
        invincible = true;
        invTimer = INVINCIBLE_DURATION;
    }
}

void Player::update(float dt, ResourceManager& rm, const InputState& input, std::vector<Bullet>& outBullets)
{
    // 이동 처리
	sf::Vector2f move(0.f, 0.f);
	if (input.up)    move.y -= PLAYER_SPEED;
	if (input.down)  move.y += PLAYER_SPEED;
	if (input.left)  move.x -= PLAYER_SPEED;
	if (input.right) move.x += PLAYER_SPEED;
	s.move(move * dt);

    ClampPlayerToScreen(); 

    fireTimer += dt;
    if (fireTimer > 0.2f)
    {
        fireTimer = 0.f;
        outBullets.emplace_back(rm, "player_bullet.png", getPosition(), sf::Vector2f(BULLET_SPEED, 0.f));
    }

    anim->update(dt);

    hitEffect(dt); 
    invinEffect(dt); 
}

// 플레이어가 화면 밖으로 나가지 않도록 제한
void Player::ClampPlayerToScreen()
{
    if (getPosition().x < 0.f)
        setPosition(0.f, getPosition().y);
    if (getPosition().x > WINDOW_WIDTH - s.getGlobalBounds().width)
        setPosition(WINDOW_WIDTH - s.getGlobalBounds().width, getPosition().y);
    if (getPosition().y < 0.f)
        setPosition(getPosition().x, 0.f);
    if (getPosition().y > WINDOW_HEIGHT - s.getGlobalBounds().height)
        setPosition(getPosition().x, WINDOW_HEIGHT - s.getGlobalBounds().height);
}

void Player::draw(sf::RenderWindow& window)
{
    if (invincible)
    {
        shieldSprite.setPosition(getPosition() + sf::Vector2f(s.getGlobalBounds().width / 2.f, s.getGlobalBounds().height / 2.f));		
        window.draw(shieldSprite); 
        window.draw(s);
        return;
	}
    else if (hitTimer > 0.f)
    {
        hitSprite.setPosition(getPosition());
        window.draw(hitSprite);
    }
    else
    {
        window.draw(s);
    }
}