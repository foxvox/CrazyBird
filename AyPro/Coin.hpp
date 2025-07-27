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
			v.y += 9.81f * wv * dt; // 중력 적용 
            s.move(v * dt);            
            float d = hypot(playerPos.x - s.getPosition().x, playerPos.y - s.getPosition().y);
            if (d < COIN_ATTRACT_RADIUS) collected = true;
        }
		else // 코인이 수집된 상태라면 코인이 플레이어를 향해 이동합니다.
        {
            sf::Vector2f dir = playerPos - s.getPosition();
            float len = std::hypot(dir.x, dir.y); 
            // 최소 거리 임계값 이 조건이 없으면 필셀 떨림 현상이 발생할 수 있습니다. 
            if (len >= 5.f)
            {
                // 스프라이트를 한 프레임만큼 해당 방향으로 이동시킵니다.
                s.move(dir / len * COIN_ATTRACT_SPEED * dt);
            }                
        }

        anim->update(dt);
    }
};