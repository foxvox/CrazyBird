#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <memory>
#include <string>

class SoundManager
{
private:
    std::map<std::string, std::unique_ptr<sf::Music>> musicMap;
    std::string currentMusic;

public:
    // 사운드 로드
    void load(const std::string& name, const std::string& filename)
    {
        auto music = std::make_unique<sf::Music>();
        if (music->openFromFile(filename))
        {
            music->setLoop(true);
            musicMap[name] = std::move(music);
        }
    }

    // 음악 재생
    void play(const std::string& name)
    {
        if (currentMusic == name) return;

        // 현재 재생 중 음악 멈추기
        if (musicMap.count(currentMusic))
            musicMap[currentMusic]->stop();

        currentMusic = name;

        if (musicMap.count(name))
            musicMap[name]->play();
    }

    void stopAll()
    {
        for (auto& [name, music] : musicMap)
            music->stop();
        currentMusic.clear();
    }
};