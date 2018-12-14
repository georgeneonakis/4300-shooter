#pragma once

#include "Common.h"
#include "GameState.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

struct PlayerConfig 
{ 
	float X, Y, CX, CY, SPEED, HEALTH, DSPEED, DTIME, DCOOLDOWN;
};

struct WeaponConfig
{
	float CX, CY, SPEED, LIFESPAN, FIRERATE, DAMAGE, AMMO;
};

class GameState_Play : public GameState
{

protected:

    EntityManager           m_entityManager;
    std::shared_ptr<Entity> m_player;
	std::shared_ptr<Entity> player_aura;
    std::string             m_levelPath;
	sf::Text				m_playerHealth;
	sf::Text				m_playerShields;
	sf::Text				m_playerAmmo;
    PlayerConfig            m_playerConfig;
	WeaponConfig			m_pistolConfig;
	WeaponConfig			m_shotgunConfig;
	WeaponConfig			m_rifleConfig;
	WeaponConfig			m_launcherConfig;
	WeaponConfig			m_fragConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = false;
	bool					m_showInventory = false;
	int						m_windowX = 1280;
	int						m_windowY = 768;
    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    void update();
    void spawnPlayer();
    void fireWeapon(std::shared_ptr<Entity> entity, const Vec2 & target);
	void sSpawnMissile(std::shared_ptr<Entity> shooter, std::shared_ptr<Entity> victim);
	void startreload();
	void endreload();
	void inflictDamage(std::shared_ptr<Entity> source, std::shared_ptr<Entity> target);

	void sGravity();
	void sBuffs();
	void sSteer();
    void sMovement();
    void sAI();
    void sLifespan();
    void sUserInput();
    void sAnimation();
    void sCollision();
    void sRender();

public:

    GameState_Play(GameEngine & game, const std::string & levelPath);

};