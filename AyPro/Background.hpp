#pragma once 
#include "Common.hpp" 
#include "ResourceManager.hpp" 

class Background 
{
    sf::Sprite s1, s2;
    float speed;
public:
    Background(ResourceManager& rm, const std::string& file, float sp)
        : speed(sp) 
    {
        sf::Texture& t = rm.get(file);
        s1.setTexture(t); s2.setTexture(t);
        s1.setPosition(0, 0);
        s2.setPosition(WINDOW_WIDTH, 0);
    }

	~Background() = default;
    
    void update(float dt) 
    {
        s1.move(-speed * dt, 0);
        s2.move(-speed * dt, 0);
        if (s1.getPosition().x <= -WINDOW_WIDTH)
            s1.setPosition(s2.getPosition().x + WINDOW_WIDTH, 0); 
        if (s2.getPosition().x <= -WINDOW_WIDTH) 
            s2.setPosition(s1.getPosition().x + WINDOW_WIDTH, 0);    
    }

    void draw(sf::RenderWindow& w) 
    {
        w.draw(s1);
        w.draw(s2);
    }
};