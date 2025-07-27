#include "Player.hpp" 
#include "Game.hpp" 

void Player::hitEffect(float dt) 
{
    // �÷��̾ �¾��� �� �����̴� ȿ��  
    if (hitTimer > 0.f)
    {
        hitTimer -= dt;

        if (hitTimer <= 0.f)
        {
            hitTimer = 0.f;
            hitSprite.setColor(sf::Color::White);
            return;
        }

        // hit �ִϸ��̼� ������Ʈ �ϰ� ���� �����̴� ȿ�� ���� 
        hitAnim->update(dt);
        // blink ȿ��           
        float alpha = (std::sin(hitTimer * 20) + 1.f) * 0.5f * 255;
        hitSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
    }
}

void Player::invinEffect(float dt)
{
    // ���� ������ �� �����̴� ȿ�� 
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
        // blink ȿ�� 
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
    // �̵� ó��
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

// �÷��̾ ȭ�� ������ ������ �ʵ��� ����
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