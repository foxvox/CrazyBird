#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <thread>
#include <chrono>

class SoundEffectManager
{
private:
    std::map<std::string, sf::SoundBuffer> bufferMap;
    std::vector<sf::Sound> activeSounds;

public: 
    SoundEffectManager()
    {
        activeSounds.reserve(64); 
    }

    void load(const std::string& name, const std::string& filename)
    {
        sf::SoundBuffer buffer;
        if (buffer.loadFromFile(filename))
            bufferMap[name] = buffer;
    }

    void play(const std::string& name)
    {
        auto it = bufferMap.find(name);
        if (it == bufferMap.end()) return; 

        sf::Sound sound;
        sound.setBuffer(it->second); 
        sound.setVolume(80.f);  
        sound.play(); 

        activeSounds.push_back(std::move(sound));
    } 

    void playAsync(const std::string& name)
    {
        std::thread([this, name]() {
            this->playAndWait(name);
            }).detach();
    }

    void playAndWait(const std::string& name)
    {
        auto it = bufferMap.find(name);
        if (it == bufferMap.end()) return;

        sf::Sound sound;
        sound.setBuffer(it->second);
        sound.setVolume(100.f);
        sound.play();

        while (sound.getStatus() == sf::Sound::Playing)
        {
            sf::sleep(sf::milliseconds(100));
        }
    }

    void cleanup()
    {
        activeSounds.erase(std::remove_if(activeSounds.begin(), activeSounds.end(), [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Stopped; }), activeSounds.end());
    }
};