#pragma once
#include "Common.hpp"
#include "ResourceManager.hpp"
#include "Bullet.hpp"
#include "Animation.hpp" 

class Player
{
public:
    sf::Sprite                  s;
    sf::Vector2f                v;

private:
    int                         frames;
    float                       animSpeed;

    std::unique_ptr<Animation>  anim;
    float                       fireTimer = 0.f; 

    sf::Sprite                  hitSprite; 
	std::unique_ptr<Animation>  hitAnim; 

    sf::Sprite                  shieldSprite;

	float                       hitTimer = 0.f;
    bool                        invincible = false;
    float                       invTimer = 0.f; 

public:
    Player(ResourceManager& rm, const std::string& file, int fr)
        : frames(fr), animSpeed(0.1f)
    {
        auto& tex = rm.get(file);
        s.setTexture(tex);
        int w = tex.getSize().x / frames;
        int h = tex.getSize().y;
        s.setTextureRect({ 0, 0, w, h });
        setPosition(100.f, WINDOW_HEIGHT / 2.f); 

        anim = std::make_unique<Animation>(s, frames, animSpeed); 

        hitSprite.setTexture(rm.get("hit.png")); 
        auto& htex = rm.get("hit.png"); 
        int hw = htex.getSize().x / 2;
        int hh = htex.getSize().y; 
        hitSprite.setTextureRect({ 0, 0, hw, hh });
		hitAnim = std::make_unique<Animation>(hitSprite, 2, 0.1f);  

        // 무적 상태용 보호막 이미지 설정
        shieldSprite.setTexture(rm.get("shield.png"));
        auto& shieldTex = rm.get("shield.png");
        int sw = shieldTex.getSize().x;
        int sh = shieldTex.getSize().y;
        shieldSprite.setTextureRect({ 0, 0, sw, sh });
        shieldSprite.setOrigin(sw / 2.f, sh / 2.f);

    }

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    Player(Player&& o) noexcept
        : s(std::move(o.s))
        , v(o.v)
        , frames(o.frames)
        , animSpeed(o.animSpeed)
        , fireTimer(o.fireTimer)
        , hitSprite(std::move(o.hitSprite))
        , hitTimer(o.hitTimer)
        , invincible(o.invincible)
        , invTimer(o.invTimer)
    {
        anim = std::make_unique<Animation>(s, frames, animSpeed);
        hitAnim = std::make_unique<Animation>(hitSprite, 2, 0.1f);
    } 

    Player& operator=(Player&& o) noexcept
    {
        if (this != &o)
        {
            s          = std::move(o.s);
            v          = o.v;
            frames     = o.frames;
            animSpeed  = o.animSpeed;
            fireTimer  = o.fireTimer;
            hitSprite  = std::move(o.hitSprite);
            hitTimer   = o.hitTimer;
            invincible = o.invincible;
            invTimer    = o.invTimer;

            anim    = std::make_unique<Animation>(s, frames, animSpeed);
            hitAnim = std::make_unique<Animation>(hitSprite, 2, 0.1f);
        }
        return *this;
    } 	

    const sf::Sprite& getShieldSprite() const { return shieldSprite; } 

    void activateInvincibility();

    void hit()
	{
		if (invincible) 
            return; 

        hitTimer = HIT_DURATION;
    }

    void ClampPlayerToScreen();
    void hitEffect(float dt); 
    void invinEffect(float dt); 
    bool isInvincible() const { return invincible; } 

	sf::Vector2f getPosition() const { return s.getPosition(); } 
	void         setPosition(const sf::Vector2f& pos) { s.setPosition(pos); } 
	void         setPosition(float x, float y) { s.setPosition(x, y); }     
    
    void update(float dt, ResourceManager& rm, const InputState& input, std::vector<Bullet>& outBullets);
    void draw(sf::RenderWindow& window);
}; 