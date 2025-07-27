#pragma once 

#include "Common.hpp" 
#include "ResourceManager.hpp" 
#include "Animation.hpp"

class Coin 
{
public:
    sf::Sprite s;
    sf::Vector2f v;
    bool collected = false;
    std::unique_ptr<Animation> anim;

    Coin(ResourceManager& rm, const sf::Vector2f& pos)
        : v{ -COIN_BOUNCE_SPEED,-COIN_BOUNCE_SPEED }
    {
        auto& tex = rm.get("coin.png");
        s.setTexture(tex);
        int w = tex.getSize().x / 8;
        int h = tex.getSize().y;
        s.setTextureRect({ 0,0,w,h });
        s.setPosition(pos);

        anim = std::make_unique<Animation>(s, 8, 0.1f);        
    }

    Coin(Coin&& o) noexcept
        : s(std::move(o.s)), v(o.v), collected(o.collected)
    {
        anim = std::make_unique<Animation>(s, 8, 0.1f);
    }

    Coin& operator=(Coin&& o) noexcept 
    {
        if (this != &o) {
            s = std::move(o.s);
            v = o.v;
            collected = o.collected;
            anim = std::make_unique<Animation>(s, 8, 0.1f);
        }
        return *this;
    }

    Coin(const Coin& o) = delete;
    Coin& operator=(const Coin& o) = delete; 

    void update(float dt, const sf::Vector2f& playerPos) 
    {
        if (!collected) 
        {
            float wv = 50.f;
			v.y += 9.81f * wv * dt; // �߷� ���� 
            s.move(v * dt);            
            float d = hypot(playerPos.x - s.getPosition().x, playerPos.y - s.getPosition().y);
            if (d < COIN_ATTRACT_RADIUS) collected = true;
        }
		else // ������ ������ ���¶�� ������ �÷��̾ ���� �̵��մϴ�.
        {
            sf::Vector2f dir = playerPos - s.getPosition();
            float len = std::hypot(dir.x, dir.y); 
            // �ּ� �Ÿ� �Ӱ谪 �� ������ ������ �ʼ� ���� ������ �߻��� �� �ֽ��ϴ�. 
            if (len >= 5.f)
            {
                // ��������Ʈ�� �� �����Ӹ�ŭ �ش� �������� �̵���ŵ�ϴ�.
                s.move(dir / len * COIN_ATTRACT_SPEED * dt);
            }                
        }

        anim->update(dt);
    }
};