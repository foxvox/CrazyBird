#pragma once 
#include "Common.hpp"  
#include "ResourceManager.hpp" 
#include "Animation.hpp" 

class Bullet 
{
public:
    sf::Sprite                  s;
    sf::Vector2f                v;
    bool                        alive = true; 

    std::unique_ptr<Animation>  anim;
    int                         frameCount = 1;

    Bullet(ResourceManager& rm, const std::string& file, const sf::Vector2f& pos, const sf::Vector2f& vel, int frames = 1, float frameTime = 0.f) 
        : v(vel), frameCount(frames) 
    {
        sf::Texture& tex = rm.get(file);
        s.setTexture(tex);

        int tw = tex.getSize().x / frames;
        int th = tex.getSize().y;
        s.setTextureRect({ 0, 0, tw, th });
        s.setPosition(pos);
        // 애니메이션 객체 동적 생성
        anim = std::make_unique<Animation>(s, frames, frameTime);
    }

    // 복사 불가, 이동만 허용
    Bullet(const Bullet&) = delete;
    Bullet& operator=(const Bullet&) = delete;
    Bullet(Bullet&&) noexcept = default;
    Bullet& operator=(Bullet&&) noexcept = default;

    void update(float dt) 
    {
		if (!alive) return; 

        s.move(v * dt);

        if (frameCount > 1 && anim) 
        {
            anim->update(dt);
        }

        auto pos = s.getPosition();
        if (pos.x < -50 || pos.x > WINDOW_WIDTH + 50 ||
            pos.y < -50 || pos.y > WINDOW_HEIGHT + 50)
        {
            alive = false;
        }
    }
};