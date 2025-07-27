#include "ResourceManager.hpp" 

sf::Texture& ResourceManager::get(const std::string& id)
{
    if (textures.find(id) == textures.end())
    {
        sf::Texture tex;
        tex.loadFromFile("res/" + id);
        textures[id] = tex;
    }
    return textures[id];
} 