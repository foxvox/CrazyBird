#pragma once 

#include "Common.hpp"   
#include "ResourceManager.hpp"
#include "Bullet.hpp" 

class Pet
{
public:
    sf::Sprite                 s;
    sf::Vector2f               v;
    sf::Vector2f               offset;
    std::vector<Bullet>        bullets;
    static float 			   ultimateTimer;
    bool                       active{ false };

private:
    int                        frames;
    float                      animSpeed;
    bool					   ultimate{ false };    
    std::unique_ptr<Animation> anim;
	float                      fireTimer = 0.f;

public:
    Pet(ResourceManager& rm, const std::string& file, int fr, float ox, float oy)
        : frames(fr)
        , animSpeed(0.1f)
        , offset(ox, oy)
    {
        auto& tex = rm.get(file);
        s.setTexture(tex);
        int w = tex.getSize().x / frames;
        int h = tex.getSize().y;
        s.setTextureRect({ 0, 0, w, h }); 

        anim = std::make_unique<Animation>(s, frames, animSpeed);
    }  
    Pet(Pet&& o) noexcept
        : s(std::move(o.s))
        , v(o.v)
        , frames(o.frames)
        , animSpeed(o.animSpeed)
    {
        anim = std::make_unique<Animation>(s, frames, animSpeed);
    }    
    Pet& operator=(Pet&& o) noexcept 
    {
        if (this != &o) 
        {
            s = std::move(o.s);
            v = o.v;
            frames = o.frames;
            animSpeed = o.animSpeed;
            anim = std::make_unique<Animation>(s, frames, animSpeed);
        }
        return *this;
    }

    Pet(const Pet&) = delete;
    Pet& operator=(const Pet&) = delete; 

    bool isUltimate() const; 
    void ultimateEffect(float dt, ResourceManager& rm); 
    void activateUltimate();
    void update(float dt, const sf::Vector2f& playerPos, ResourceManager& rm, std::vector<Bullet>& outBullets);     
    void draw(sf::RenderWindow& window);
};