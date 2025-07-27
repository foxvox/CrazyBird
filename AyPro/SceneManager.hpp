#pragma once 
#include "Common.hpp"   
#include "ResourceManager.hpp" 

enum class GameState { START, PLAY }; 

class SceneManager 
{
    ResourceManager     rm;
    GameState           state{ GameState::START };
    
    sf::RenderWindow    window;
    sf::Sprite          startBg, button;
public:
	SceneManager();
    void run(); 
};