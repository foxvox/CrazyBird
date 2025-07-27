#pragma once 

#include "Common.hpp"   

class ResourceManager
{
private:
    std::map<std::string, sf::Texture> textures;

public:
    sf::Texture& get(const std::string& id); 
}; 