#pragma once 
#include "Common.hpp"  
#include "ResourceManager.hpp" 
#include "Animation.hpp" 

class Enemy 
{
public:
    sf::Sprite                  s;
    sf::Vector2f                v;
    bool                        alive = true;

private:
    int                         frames;
    float                       animSpeed;
    std::unique_ptr<Animation>  anim;

public:
    Enemy(ResourceManager& rm, const std::string& file, int fr, float angleVariance)
        : frames(fr)
        , animSpeed(0.1f)
    {
        auto& tex = rm.get(file);
        s.setTexture(tex);
        int w = tex.getSize().x / frames;
        int h = tex.getSize().y;
        s.setTextureRect({ 0, 0, w, h });

		// raw means random angle width, raw에는 -angleVariance ~ angleVariance 사이의 값이 들어감 
        float raw = static_cast<float>(std::rand() % int(angleVariance * 2 * 100) - angleVariance * 100) / 100.f;
        float ang = raw * PI / 180.f; 
		// 원점 대칭시킨 속도 벡터 
        v = { -ENEMY_SPEED * std::cos(ang), 
              -ENEMY_SPEED * std::sin(ang) };

        s.setPosition(WINDOW_WIDTH + 50, std::rand() % WINDOW_HEIGHT);

        anim = std::make_unique<Animation>(s, frames, animSpeed);
    }    
    
    Enemy(Enemy&& o) noexcept
        : s(std::move(o.s))
        , v(o.v)
        , alive(o.alive)
        , frames(o.frames)
        , animSpeed(o.animSpeed)
    {
        anim = std::make_unique<Animation>(s, frames, animSpeed);
    }    
    Enemy& operator=(Enemy&& o) noexcept 
    {
        if (this != &o) 
        {
            s = std::move(o.s);
            v = o.v;
            alive = o.alive;
            frames = o.frames;
            animSpeed = o.animSpeed;
            anim = std::make_unique<Animation>(s, frames, animSpeed); 
        }
        return *this;
    }    
    Enemy(const Enemy&) = delete;
    Enemy& operator=(const Enemy&) = delete;

    void update(float dt) 
    {
        s.move(v * dt);
        anim->update(dt);
        if (s.getPosition().x < -50) alive = false;
    }
};