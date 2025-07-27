#include "SceneManager.hpp" 
#include "Game.hpp" 
#include "SoundEffectManager.hpp" 
#include "SoundManager.hpp" 

extern SoundManager         sm;         // ���� SoundManager �ν��Ͻ�
extern SoundEffectManager   sem;        // ���� SoundEffectManager �ν��Ͻ�

SceneManager::SceneManager() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Crazy Bird")
{
    sm.load("bg", "bg_sound.ogg");
    startBg.setTexture(rm.get("start.jpg"));
    button.setTexture(rm.get("button.png"));
    button.setPosition((WINDOW_WIDTH - button.getGlobalBounds().width) / 2, (WINDOW_HEIGHT - button.getGlobalBounds().height) / 2); 
    sem.load("ready", "ready_sound.wav"); 
    sem.playAsync("ready"); 
    state = GameState::START; 
}

void SceneManager::run()
{
    while (window.isOpen()) 
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();

            if (state == GameState::START && e.type == sf::Event::MouseButtonPressed)
            {
                auto mp = sf::Mouse::getPosition(window);
                if (button.getGlobalBounds().contains(mp.x, mp.y))
                {
                    state = GameState::PLAY;                    
                    Game game(rm);
                    game.run();
                    state = GameState::START;
                }
            }
        }

        if (state == GameState::START)
        {
            sm.play("bg");
			Game::coinCount = 0; // ���� ���� �� ���� �� �ʱ�ȭ 
            window.clear();
            window.draw(startBg);
            window.draw(button);
            window.display();
        }
    } // ���� ���� ����
} // void run()