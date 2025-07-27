#pragma once 

class Animation 
{
public: 
    sf::Sprite&     s;
    int             frames;
    float           speed;
    int             current = 0;
    float           timer = 0;

public:
    Animation(sf::Sprite& spr, int f, float sp) : s(spr), frames(f), speed(sp) {} 

    Animation(const Animation&) = delete;
    Animation& operator=(const Animation&) = delete;
    Animation(Animation&&) noexcept = default;
    Animation& operator=(Animation&&) noexcept = default;

    void update(float dt) 
    {
        timer += dt;
        if (timer >= speed) 
        {
            timer = 0;
            current = (current + 1) % frames;
            int w = s.getTexture()->getSize().x / frames;
            s.setTextureRect({ current * w,0,w,(int)s.getTexture()->getSize().y });
        }
    }
}; 