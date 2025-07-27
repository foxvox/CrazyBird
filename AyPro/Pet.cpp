#include "Pet.hpp"  
#include "Game.hpp"  

float Pet::ultimateTimer = 0.f; 

bool Pet::isUltimate() const
{
    return ultimate;
}

void Pet::activateUltimate()
{
	if (!ultimate && Game::coinCount >= COINS_FOR_ULTIMATE)
	{
		ultimate = true;
		ultimateTimer = 0.f;		
	}
}

void Pet::ultimateEffect(float dt, ResourceManager& rm)
{
	ultimateTimer += dt;
	if (ultimateTimer > ULTIMATE_DURATION)
	{
		ultimate = false;
		ultimateTimer = 0.f; 
		return;
	}

	float alpha = (std::sin(ultimateTimer * 20) + 1.f) * 0.5f * 255;
	s.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
}

void Pet::update(float dt, const sf::Vector2f& playerPos, ResourceManager& rm, std::vector<Bullet>& outBullets)
{
	if (active) // 활성화된 상태에서만 총알 발사 
	{
		fireTimer += dt;
		if (fireTimer > 0.2f)
		{
			fireTimer = 0.f;
			outBullets.emplace_back(rm, "player_bullet.png", s.getPosition(), sf::Vector2f(BULLET_SPEED, 0.f));
		}
	}

    s.setPosition(playerPos + offset);
    anim->update(dt);

	// 궁상태면 궁효과 적용 
    if (ultimate) 
        ultimateEffect(dt, rm);
    else
		s.setColor(sf::Color::White);     
} 

void Pet::draw(sf::RenderWindow& window)
{
	window.draw(s);
}

