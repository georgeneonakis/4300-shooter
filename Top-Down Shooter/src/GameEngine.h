#pragma once

#include "Common.h"
#include "GameState.h"
#include "Assets.h"

#include <memory>

class GameEngine
{

protected:

    std::vector<std::shared_ptr<GameState>> m_states;
    std::vector<std::shared_ptr<GameState>> m_statesToPush;
    sf::RenderWindow                        m_window;
    Assets                                  m_assets;
    size_t                                  m_popStates = 0;
    bool                                    m_running = true;
	bool									m_shotgun = false;
	bool									m_rifle = false;
	bool									m_launcher = false;

    void init(const std::string & path);
    void update();

    

public:
    
    GameEngine(const std::string & path);

    void pushState(std::shared_ptr<GameState> state);
    void popState();

    void quit();
    void run();

    sf::RenderWindow & window();
    bool isRunning();

	bool hasShotgun();
	bool hasRifle();
	bool hasLauncher();

	void getShotgun();
	void getRifle();
	void getLauncher();

    const Assets & getAssets() const;
};