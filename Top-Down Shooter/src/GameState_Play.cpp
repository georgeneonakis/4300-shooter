#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Inventory.h"
#include <iostream>



GameState_Play::GameState_Play(GameEngine & game, const std::string & levelPath)
    : GameState(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void GameState_Play::init(const std::string & levelPath)
{
    loadLevel(levelPath);
}

// Load the level from the config file
void GameState_Play::loadLevel(const std::string & filename)
{
    m_entityManager = EntityManager();
	std::ifstream fin(filename);
	std::string start, empty;

	while (std::getline(fin, start, ' '))
	{
		// Terrain blocks
		if (start == "Tile")
		{
			auto block = m_entityManager.addEntity("Tile");
			std::string name;
			int rx, ry, tx, ty, bm, bv;
			fin >> name >> rx >> ry >> tx >> ty >> bm >> bv;

			block->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			block->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			block->addComponent<CBoundingBox>(block->getComponent<CAnimation>()->animation.getSize(), bm, bv);
		}

		// Gravity objects
		else if (start == "Gravity")
		{
			auto grav = m_entityManager.addEntity("Gravity");
			std::string name;
			float rx, ry, tx, ty, force, radius;
			fin >> name >> rx >> ry >> tx >> ty >> force >> radius;

			grav->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			grav->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			grav->addComponent<CBoundingBox>(grav->getComponent<CAnimation>()->animation.getSize(), false, false);
			grav->addComponent<CGravity>(force, radius);
		}

		// Moving spike blocks
		else if (start == "Spike")
		{
			auto block = m_entityManager.addEntity("Spike");
			std::string name,aiType;
			float rx, ry, tx, ty, bm, bv, speed, damage;
			fin >> name >> rx >> ry >> tx >> ty >> speed >> damage >> aiType;

			block->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			block->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			block->addComponent<CBoundingBox>(block->getComponent<CAnimation>()->animation.getSize(), true, true);
			block->addComponent<CDamage>(damage, 500);

			int n;
			std::vector<Vec2> positions;
			fin >> n;

			for (int i = 0; i < n; i++)
			{
				int xi, yi;
				fin >> xi >> yi;
				positions.push_back(Vec2(xi * 64 + m_windowX * rx, yi * 64 + m_windowY * ry));
			}
			block->addComponent<CPatrol>(positions, speed);
		}

		// Health potion
		else if (start == "HPot")
		{
			auto pot = m_entityManager.addEntity("HealthPot");
			std::string name;
			float rx, ry, tx, ty;
			fin >> name >> rx >> ry >> tx >> ty;

			pot->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			pot->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			pot->addComponent<CBoundingBox>(pot->getComponent<CAnimation>()->animation.getSize(), false, false);
		}

		// Shield potion
		else if (start == "SPot")
		{
			auto pot = m_entityManager.addEntity("ShieldPot");
			std::string name;
			int rx, ry, tx, ty;
			fin >> name >> rx >> ry >> tx >> ty;

			pot->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			pot->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			pot->addComponent<CBoundingBox>(pot->getComponent<CAnimation>()->animation.getSize(), false, false);
		}

		// Speed potion
		else if (start == "Speed")
		{
			auto pot = m_entityManager.addEntity("SpeedPot");
			std::string name;
			int rx, ry, tx, ty;
			fin >> name >> rx >> ry >> tx >> ty;

			pot->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			pot->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			pot->addComponent<CBoundingBox>(pot->getComponent<CAnimation>()->animation.getSize(), false, false);
		}

		// Stealth potion
		else if (start == "Stealth")
		{
			auto pot = m_entityManager.addEntity("StealthPot");
			std::string name;
			int rx, ry, tx, ty;
			fin >> name >> rx >> ry >> tx >> ty;

			pot->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			pot->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			pot->addComponent<CBoundingBox>(pot->getComponent<CAnimation>()->animation.getSize(), false, false);
		}

		// Shotgun weapon
		else if (start == "ShotgunPickup")
		{
			auto shotgun = m_entityManager.addEntity("Shotgun");
			std::string name;
			int rx, ry, tx, ty;
			fin >> name >> rx >> ry >> tx >> ty;

			shotgun->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			shotgun->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			shotgun->addComponent<CBoundingBox>(shotgun->getComponent<CAnimation>()->animation.getSize(), false, false);
		}

		// Auto rifle weapon
		else if (start == "RiflePickup")
		{
			auto rifle = m_entityManager.addEntity("Rifle");
			std::string name;
			int rx, ry, tx, ty;
			fin >> name >> rx >> ry >> tx >> ty;

			rifle->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			rifle->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			rifle->addComponent<CBoundingBox>(rifle->getComponent<CAnimation>()->animation.getSize(), false, false);
		}

		// Grenade launcher weapon
		else if (start == "LauncherPickup")
		{
			auto launcher = m_entityManager.addEntity("Launcher");
			std::string name;
			int rx, ry, tx, ty;
			fin >> name >> rx >> ry >> tx >> ty;

			launcher->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			launcher->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			launcher->addComponent<CBoundingBox>(launcher->getComponent<CAnimation>()->animation.getSize(), false, false);
		}

		// NPCs
		else if (start == "NPC")
		{
			auto npc = m_entityManager.addEntity("NPC");
			std::string name, aiType;
			float rx, ry, tx, ty, speed, damage, health;
			fin >> name >> rx >> ry >> tx >> ty >> speed >> damage >> health >> aiType;

			npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			npc->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			npc->addComponent<CBoundingBox>(npc->getComponent<CAnimation>()->animation.getSize(), true, false);
			npc->addComponent<CHealth>(health, 0);
			npc->addComponent<CDamage>(damage, 500);

			// Follow AI
			if (aiType == "Follow")
			{
				npc->addComponent<CFollowPlayer>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64), speed);
			}
			
			// Patrol AI
			else
			{
				int n;
				std::vector<Vec2> positions;
				fin >> n;

				for (int i = 0; i < n; i++)
				{
					int xi, yi;
					fin >> xi >> yi;
					positions.push_back(Vec2(xi * 64 + m_windowX * rx, yi * 64 + m_windowY * ry));
				}
				npc->addComponent<CPatrol>(positions, speed);
			}
		}

		// Player config
		else if (start == "Player")
		{
			fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.HEALTH >>
				m_playerConfig.DTIME >> m_playerConfig.DCOOLDOWN >> m_playerConfig.DSPEED >> m_playerConfig.BUFFTIME;
		}

		// Pistol config
		else if (start == "Pistol")
		{
			fin >> m_pistolConfig.CX >> m_pistolConfig.CY >> m_pistolConfig.SPEED >> m_pistolConfig.LIFESPAN >> m_pistolConfig.DAMAGE >> m_pistolConfig.AMMO;
		}

		// Shotgun config
		else if (start == "Shotgun")
		{
			fin >> m_shotgunConfig.CX >> m_shotgunConfig.CY >> m_shotgunConfig.SPEED >> m_shotgunConfig.LIFESPAN >> m_shotgunConfig.FIRERATE >> m_shotgunConfig.DAMAGE >> m_shotgunConfig.AMMO;
		}

		// Rifle config
		else if (start == "Rifle")
		{
			fin >> m_rifleConfig.CX >> m_rifleConfig.CY >> m_rifleConfig.SPEED >> m_rifleConfig.LIFESPAN >> m_rifleConfig.FIRERATE >> m_rifleConfig.DAMAGE >> m_rifleConfig.AMMO;
		}

		// Grenade launcher config
		else if (start == "Launcher")
		{
			fin >> m_launcherConfig.CX >> m_launcherConfig.CY >> m_launcherConfig.SPEED >> m_launcherConfig.LIFESPAN >> m_launcherConfig.FIRERATE >> m_launcherConfig.AMMO;
		}

		// Frag bullet config
		else if (start == "Frag")
		{
			fin >> m_fragConfig.CX >> m_fragConfig.CY >> m_fragConfig.SPEED >> m_fragConfig.LIFESPAN >> m_fragConfig.FIRERATE >> m_fragConfig.DAMAGE;
		}

		getline(fin, empty, '\n');
	}

	// Initialize HUD text elements
	m_playerHealth.setFont(m_game.getAssets().getFont("Megaman"));
	m_playerHealth.setCharacterSize(32);
	m_playerHealth.setColor(sf::Color::Red);

	m_playerShields.setFont(m_game.getAssets().getFont("Megaman"));
	m_playerShields.setCharacterSize(32);
	m_playerShields.setColor(sf::Color::Blue);

	m_playerAmmo.setFont(m_game.getAssets().getFont("Megaman"));
	m_playerAmmo.setCharacterSize(32);
	m_playerAmmo.setColor(sf::Color::Black);

	m_pistolSound.setBuffer(m_game.getAssets().getSound("Pistol"));
	m_shotgunSound.setBuffer(m_game.getAssets().getSound("Shotgun"));
	m_rifleSound.setBuffer(m_game.getAssets().getSound("Rifle"));
	m_launcherSound.setBuffer(m_game.getAssets().getSound("Launcher"));
	m_fragSound.setBuffer(m_game.getAssets().getSound("Frag"));

    spawnPlayer();
}

// Spawn the player
void GameState_Play::spawnPlayer()
{
	// Assign all components
    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("PlayerSt"), true);
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), false, false);
    m_player->addComponent<CInput>();
	m_player->addComponent<CWeapons>(m_game.hasShotgun(), m_game.hasRifle(), m_game.hasLauncher());
	m_player->addComponent<CHealth>(m_playerConfig.HEALTH, 100);
	m_player->addComponent<CAura>(0);
	m_player->addComponent<CInventory>();
	m_player->addComponent<CDash>(m_playerConfig.DTIME, m_playerConfig.DCOOLDOWN, m_playerConfig.DSPEED);
	m_player->addComponent<CBuffs>(m_playerConfig.BUFFTIME, m_playerConfig.BUFFTIME);

	// Initialize player aura
	player_aura = m_entityManager.addEntity("Tile");
	player_aura->addComponent<CTransform>(Vec2(m_player->getComponent<CTransform>()->pos));
	player_aura->addComponent<CAnimation>(m_game.getAssets().getAnimation("NoAura"), true);
	player_aura->addComponent<CBoundingBox>(Vec2(2, 2), false, false);
}

// Fire the current weapon at the cursor
void GameState_Play::fireWeapon(std::shared_ptr<Entity> entity, const Vec2 & target)
{
	// Calculate normal vector between player and cursor
	auto weapon = m_player->getComponent<CWeapons>();
	Vec2 difference = target - entity->getComponent<CTransform>()->pos;
	float distance = target.dist(entity->getComponent<CTransform>()->pos);
	Vec2 normal = difference / distance;
	if (weapon->canShoot)
	{
		m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("PlayerA"), true);
	}

	// Fire pistol
	if (weapon->current == 1 && weapon->canShoot && m_player->getComponent<CWeapons>()->startreload == false)
	{
		if(m_player->getComponent<CWeapons>()->pistol_ammo > 0)
		{
			m_pistolSound.play();
			m_player->getComponent<CWeapons>()->pistol_ammo--;
			auto bullet = m_entityManager.addEntity("Bullet");
			bullet->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>()->pos), Vec2(m_pistolConfig.SPEED * normal.x, m_pistolConfig.SPEED * normal.y),
				Vec2(1, 1), m_player->getComponent<CTransform>()->angle);
			bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("P_bullet"), true);
			bullet->addComponent<CBoundingBox>(Vec2(m_pistolConfig.CX, m_pistolConfig.CY), false, false);
			bullet->addComponent<CLifeSpan>(m_pistolConfig.LIFESPAN);
			bullet->addComponent<CDamage>(m_pistolConfig.DAMAGE);
		}
		else
		{
			startreload();
		}
	}

	// Fire shotgun
	else if (weapon->current == 2 && weapon->clock.getElapsedTime().asMilliseconds() > weapon->nextFire)
	{
		int theta = atan2(normal.y, normal.x) * 180 / 3.14159;
		if (m_player->getComponent<CWeapons>()->shotgun_ammo > 0 && m_player->getComponent<CWeapons>()->startreload == false)
		{
			m_shotgunSound.play();
			m_player->getComponent<CWeapons>()->shotgun_ammo--;

			// Calculate trajectory for each bullet in the cone
			for (int i = -16; i <= 16; i += 4)
			{
				int newTheta = theta + i;
				Vec2 newNormal = Vec2(cos(newTheta * 3.14159 / 180), sin(newTheta * 3.14159 / 180));
				auto bullet = m_entityManager.addEntity("Bullet");
				bullet->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>()->pos), Vec2(m_shotgunConfig.SPEED * newNormal.x, m_shotgunConfig.SPEED * newNormal.y),
					Vec2(1, 1), m_player->getComponent<CTransform>()->angle);
				bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("S_bullet"), true);
				bullet->addComponent<CBoundingBox>(Vec2(m_shotgunConfig.CX, m_shotgunConfig.CY), false, false);
				bullet->addComponent<CLifeSpan>(m_shotgunConfig.LIFESPAN);
				bullet->addComponent<CDamage>(m_shotgunConfig.DAMAGE);
			}
			weapon->nextFire = weapon->clock.getElapsedTime().asMilliseconds() + m_shotgunConfig.FIRERATE;
		}
		else
		{
			startreload();
		}
	}

	// Fire rifle
	else if (weapon->current == 3 && weapon->clock.getElapsedTime().asMilliseconds() > weapon->nextFire)
	{
		if (m_player->getComponent<CWeapons>()->rifle_ammo>0 && m_player->getComponent<CWeapons>()->startreload == false)
		{
			m_rifleSound.play();
			m_player->getComponent<CWeapons>()->rifle_ammo--;
			auto bullet = m_entityManager.addEntity("Bullet");
			bullet->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>()->pos), Vec2(m_rifleConfig.SPEED * normal.x, m_rifleConfig.SPEED * normal.y),
				Vec2(1, 1), m_player->getComponent<CTransform>()->angle);
			bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("R_bullet"), true);
			bullet->addComponent<CBoundingBox>(Vec2(m_rifleConfig.CX, m_rifleConfig.CY), false, false);
			bullet->addComponent<CLifeSpan>(m_rifleConfig.LIFESPAN);
			bullet->addComponent<CDamage>(m_rifleConfig.DAMAGE);
			weapon->nextFire = weapon->clock.getElapsedTime().asMilliseconds() + m_rifleConfig.FIRERATE;
		}
		else
		{
			startreload();
		}
	}

	// Fire grenade launcher
	else if (weapon->current == 4 && weapon->clock.getElapsedTime().asMilliseconds() > weapon->nextFire)
	{
		if (m_player->getComponent<CWeapons>()->launcher_ammo > 0 && m_player->getComponent<CWeapons>()->startreload == false)
		{
			m_launcherSound.play();
			m_player->getComponent<CWeapons>()->launcher_ammo--;
			auto grenade = m_entityManager.addEntity("Grenade");
			grenade->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>()->pos), Vec2(m_launcherConfig.SPEED * normal.x, m_launcherConfig.SPEED * normal.y),
				Vec2(1, 1), m_player->getComponent<CTransform>()->angle);
			grenade->addComponent<CAnimation>(m_game.getAssets().getAnimation("Frag"), true);
			grenade->addComponent<CBoundingBox>(Vec2(m_launcherConfig.CX, m_launcherConfig.CY), false, false);
			grenade->addComponent<CLifeSpan>(m_launcherConfig.LIFESPAN);
			weapon->nextFire = weapon->clock.getElapsedTime().asMilliseconds() + m_launcherConfig.FIRERATE;
		}
		else
		{
			startreload();
		}
	}
}

// Start reloading weapons
void GameState_Play::startreload() 
{

	if (m_player->getComponent<CWeapons>()->startreload == false) 
	{
		m_player->getComponent<CWeapons>()->currtime = m_player->getComponent<CWeapons>()->clock.getElapsedTime().asMilliseconds();
		m_player->getComponent<CWeapons>()->startreload = true;
	}

}

// Finish reloading
void GameState_Play::endreload()
{
	int check = m_player->getComponent<CWeapons>()->currtime;

	if (m_player->getComponent<CWeapons>()->clock.getElapsedTime().asMilliseconds() > check + 2000 && m_player->getComponent<CWeapons>()->startreload)
	{
		m_player->getComponent<CWeapons>()->startreload = false;
		m_player->getComponent<CWeapons>()->pistol_ammo = m_pistolConfig.AMMO;
		m_player->getComponent<CWeapons>()->shotgun_ammo = m_shotgunConfig.AMMO;
		m_player->getComponent<CWeapons>()->rifle_ammo = m_rifleConfig.AMMO;
		m_player->getComponent<CWeapons>()->launcher_ammo = m_launcherConfig.AMMO;
	}
}

// Deal damage to an entity
void GameState_Play::inflictDamage(std::shared_ptr<Entity> source, std::shared_ptr<Entity> target)
{
	auto damage = source->getComponent<CDamage>();
	auto health = target->getComponent<CHealth>();
	if (damage->clock.getElapsedTime().asMilliseconds() >= damage->nextFire)
	{
		damage->nextFire = damage->clock.getElapsedTime().asMilliseconds() + damage->fireRate;
		if (health->currentShield > 0)
		{
			if (health->currentShield >= damage->damage)
			{
				health->currentShield -= damage->damage;
				return;
			}
			else
			{
				health->currentHP -= damage->damage - health->currentShield;
				health->currentShield = 0;
			}
		}
		else
		{
			health->currentHP -= damage->damage;
		}
		if (health->currentHP <= 0)
		{
			if (target->tag() == "Player" && m_player->getComponent<CAnimation>()->animation.getName() != "PlayerDeath")
			{
				m_player->removeComponent<CAnimation>();
				m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("PlayerDeath"), false);
				m_player->addComponent<CBoundingBox>(Vec2(1, 1), true, false);
			//std:: cout << m_player->getComponent<CAnimation>()->animation.getName();
				//m_game.popState();
				//return;
			}
			else
			{
				target->destroy();
			}
		}
	}
}


void GameState_Play::playerdeath()
{
	int test = 1;

	for (auto e : m_entityManager.getEntities())
	{
		if (e->tag() == "Player")
		{
			test = 0;
		}

	}

	if (test == 1) { m_game.popState(); return; }


}
// Spawn a homing missile
void GameState_Play::spawnMissile(std::shared_ptr<Entity> shooter, std::shared_ptr<Entity> victim)
{
	float speed = 5.0;
	auto missile = m_entityManager.addEntity("Missile");
	Vec2 target = victim->getComponent<CTransform>()->pos;
	Vec2 current = shooter->getComponent<CTransform>()->pos;
	Vec2 difference = target - current;
	float distance = target.dist(current);
	Vec2 normal = difference / distance;
	missile->addComponent<CTransform>(Vec2(shooter->getComponent<CTransform>()->pos), Vec2(normal.x * speed, normal.y * speed),
		Vec2(1, 1), 0);
	missile->addComponent<CAnimation>(m_game.getAssets().getAnimation("Arrow"), true);
	missile->addComponent<CBoundingBox>(Vec2(m_pistolConfig.CX, m_pistolConfig.CY), false, false);
	missile->addComponent<CLifeSpan>(m_pistolConfig.LIFESPAN);
}

// Game loop
void GameState_Play::update()
{
    m_entityManager.update();

    if (!m_paused)
    {
        sAI();
		sBuffs();
        sMovement();
		sGravity();
        sLifespan();
        sCollision();
        sAnimation();
		sSteer();
		endreload();
    }

    sUserInput();
    sRender();
	playerdeath();
}

// Move entities each frame
void GameState_Play::sMovement()
{
	auto pInput = m_player->getComponent<CInput>();
	auto pTransform = m_player->getComponent<CTransform>();
	auto pDash = m_player->getComponent<CDash>();
	if (!(pDash->isDashing))
	{
		if (!(pDash->canDash) && pDash->clock.getElapsedTime().asMilliseconds() >= pDash->dashTimer)
		{
			pDash->canDash = true;
		}

		if ((pTransform->speed.x != 0 || pTransform->speed.y != 0) && m_player->getComponent<CAnimation>()->animation.getName() != "PlayerM" && m_player->getComponent<CAnimation>()->animation.getName() != "PlayerA"&& m_player->getComponent<CAnimation>()->animation.getName() != "PlayerDeath")
		{
			m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("PlayerM"), true);
		}
		else if (pTransform->speed.x == 0 && pTransform->speed.y ==0 && m_player->getComponent<CAnimation>()->animation.getName()!= "PlayerA" && m_player->getComponent<CAnimation>()->animation.getName() != "PlayerDeath")
		{
			m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("PlayerSt"), true);

		}
		pTransform->speed = Vec2(0, 0);
		// Determine player's speed based on input
		if (m_player->getComponent<CAnimation>()->animation.getName() != "PlayerDeath")
		{
			if (pInput->up)
			{
				pTransform->speed.y -= m_playerConfig.SPEED;
			}
			if (pInput->down)
			{
				pTransform->speed.y += m_playerConfig.SPEED;
			}
			if (pInput->left)
			{
				pTransform->speed.x -= m_playerConfig.SPEED;
			}
			if (pInput->right)
			{
				pTransform->speed.x += m_playerConfig.SPEED;
			}
		}
	}
	else if (pDash->clock.getElapsedTime().asMilliseconds() >= pDash->dashTimer)
	{
		pDash->isDashing = false;
		pDash->dashTimer = pDash->clock.getElapsedTime().asMilliseconds() + pDash->dashCooldown;
	}

	// Rotate player to face cursor
	Vec2 mousePosition = Vec2(sf::Mouse::getPosition(m_game.window()).x, sf::Mouse::getPosition(m_game.window()).y) - Vec2(m_windowX / 2, m_windowY / 2);
	Vec2 relativePosition = mousePosition + Vec2(m_player->getComponent<CTransform>()->pos);
	Vec2 difference = m_player->getComponent<CTransform>()->pos - relativePosition;
	m_player->getComponent<CTransform>()->angle = -atan2(difference.x, difference.y) * 180 / 3.14159;

	// Move all entities
	for (auto e : m_entityManager.getEntities())
	{
		e->getComponent<CTransform>()->prevPos = Vec2(e->getComponent<CTransform>()->pos);
		e->getComponent<CTransform>()->pos += e->getComponent<CTransform>()->speed;
		if (e->tag() == "Player")
		{
			player_aura->getComponent<CTransform>()->pos = Vec2(m_player->getComponent<CTransform>()->pos);
			player_aura->getComponent<CTransform>()->angle = float(m_player->getComponent<CTransform>()->angle);
			player_aura->getComponent<CTransform>()->scale = Vec2(m_player->getComponent<CTransform>()->scale);
		}
	}
}

// Pull entities toward a gravity source
void GameState_Play::sGravity()
{
	for (auto g : m_entityManager.getEntities("Gravity"))
	{
		auto gravity = g->getComponent<CGravity>();
		for (auto e : m_entityManager.getEntities())
		{
			if (e->tag() == "Gravity")
			{
				continue;
			}
			float distance = e->getComponent<CTransform>()->pos.dist(g->getComponent<CTransform>()->pos);
			if (distance <= gravity->radius)
			{
				Vec2 difference = e->getComponent<CTransform>()->pos - g->getComponent<CTransform>()->pos;
				Vec2 normal = difference / distance;
				e->getComponent<CTransform>()->pos -= Vec2(normal.x * gravity->gravity, normal.y * gravity->gravity);
			}
		}
	}
}

// Track item buff durations
void GameState_Play::sBuffs()
{
	auto buffs = m_player->getComponent<CBuffs>();
	if (buffs->isFast)
	{
		if (!(buffs->speedBoosted))
		{
			m_playerConfig.SPEED *= 2;
			buffs->speedBoosted = true;
		}
		if (buffs->clock.getElapsedTime().asMilliseconds() > buffs->speedTimer)
		{
			m_playerConfig.SPEED /= 2;
			buffs->isFast = false;
			buffs->speedBoosted = false;
		}
	}
	if (buffs->isStealthed && buffs->clock.getElapsedTime().asMilliseconds() > buffs->stealthTimer)
	{
		buffs->isStealthed = false;
		m_player->getComponent<CBoundingBox>()->blockVision = false;
	}
}

// Steer homing missiles
void GameState_Play::sSteer()
{
	float scale = 0.1;

	for (auto e : m_entityManager.getEntities("Missile"))
	{
		auto target = m_player->getComponent<CTransform>();
		auto current = e->getComponent<CTransform>();
		Vec2 desired = target->pos - current->pos;
		desired = desired / target->pos.dist(current->pos);
		desired = desired * current->pos.dist(current->pos + current->speed);
		Vec2 steering = desired - current->speed;
		steering = steering * scale;
		Vec2 actual = e->getComponent<CTransform>()->speed + steering;
		e->getComponent<CTransform>()->speed = actual;
		e->getComponent<CTransform>()->angle = -atan2(current->speed.x, current->speed.y) * 180 / 3.14159;
	}
}

// Generate AI behavior
void GameState_Play::sAI()
{
	// Move spike blocks
	for (auto e : m_entityManager.getEntities("Spike"))
	{
		// Patrol behavior
		if (e->hasComponent<CPatrol>())
		{
			auto patrol = e->getComponent<CPatrol>();
			// Update next position if close enough to target
			if (e->getComponent<CTransform>()->pos.dist(patrol->positions[patrol->currentPosition]) < 5.0)
			{
				patrol->currentPosition = (patrol->currentPosition + 1) % patrol->positions.size();
			}
			Vec2 nextPosition = patrol->positions[patrol->currentPosition];
			// Move toward next position
			Vec2 difference = nextPosition - e->getComponent<CTransform>()->pos;
			float distance = nextPosition.dist(e->getComponent<CTransform>()->pos);
			difference /= distance;
			difference *= patrol->speed;
			e->getComponent<CTransform>()->speed = difference;
		}
	}

	for (auto e : m_entityManager.getEntities("NPC"))
	{
		// Patrol behavior
		if (e->hasComponent<CPatrol>())
		{
			auto patrol = e->getComponent<CPatrol>();
			// Update next position if close enough to target
			if (e->getComponent<CTransform>()->pos.dist(patrol->positions[patrol->currentPosition]) < 5.0)
			{
				patrol->currentPosition = (patrol->currentPosition + 1) % patrol->positions.size();
			}
			Vec2 nextPosition = patrol->positions[patrol->currentPosition];
			// Move toward next position
			Vec2 difference = nextPosition - e->getComponent<CTransform>()->pos;
			float distance = nextPosition.dist(e->getComponent<CTransform>()->pos);
			difference /= distance;
			difference *= patrol->speed;
			e->getComponent<CTransform>()->speed = difference;
		}
		// Follow behavior
		else
		{
			auto follow = e->getComponent<CFollowPlayer>();
			bool blocked = false;
			// Check for raycast intersection with all entities
			for (auto thing : m_entityManager.getEntities())
			{
				// Only check for intersection if the entity blocks vision
				if (thing->getComponent<CBoundingBox>()->blockVision)
				{
					blocked = Physics::EntityIntersect(e->getComponent<CTransform>()->pos,
						m_player->getComponent<CTransform>()->pos, thing);
				}
				if (blocked) { break; }
			}
			// If vision is blocked, go back to home position
			if (blocked)
			{
				// If close enough to home, stop moving
				if (e->getComponent<CTransform>()->pos.dist(follow->home) < 5.0)
				{
					e->getComponent <CTransform>()->speed = Vec2(0, 0);
					continue;
				}
				// Else move toward home position
				Vec2 difference = follow->home - e->getComponent<CTransform>()->pos;
				float distance = follow->home.dist(e->getComponent<CTransform>()->pos);
				difference /= distance;
				difference *= follow->speed;
				e->getComponent<CTransform>()->speed = difference;
			}
			// If vision is not blocked, pursue the player
			else
			{
				Vec2 difference = m_player->getComponent<CTransform>()->pos - e->getComponent<CTransform>()->pos;
				float distance = m_player->getComponent<CTransform>()->pos.dist(e->getComponent<CTransform>()->pos);
				difference /= distance;
				difference *= follow->speed;
				e->getComponent<CTransform>()->speed = difference;
			}
		}
	}
}

// Track entity lifespans and destroy them if they expire
void GameState_Play::sLifespan()
{
	for (auto e : m_entityManager.getEntities("Missile"))
	{
		int elap = e->getComponent<CLifeSpan>()->clock.getElapsedTime().asMilliseconds();
		int life = e->getComponent<CLifeSpan>()->lifespan;

		if (elap > life)
		{
			e->destroy();
		}
	}
	for (auto e : m_entityManager.getEntities("Bullet"))
	{
		int elap = e->getComponent<CLifeSpan>()->clock.getElapsedTime().asMilliseconds();
		int life = e->getComponent<CLifeSpan>()->lifespan;

		if (elap > life)
		{
			e->destroy();
		}
	}
	// If a grenade expires, detonate it and release frag bullets
	for (auto e : m_entityManager.getEntities("Grenade"))
	{
		int elap = e->getComponent<CLifeSpan>()->clock.getElapsedTime().asMilliseconds();
		int life = e->getComponent<CLifeSpan>()->lifespan;

		if (elap > life)
		{
			m_fragSound.play();
			Vec2 source = e->getComponent<CTransform>()->pos;
			for (int i = 1; i <= m_fragConfig.FIRERATE; i++)
			{
				Vec2 newPosition = Vec2(source.x + cos(360 / m_fragConfig.FIRERATE * i * 3.1415926 / 180), source.y + sin(360 / m_fragConfig.FIRERATE * i * 3.1415926 / 180));
				Vec2 difference = newPosition - source;
				float distance = source.dist(newPosition);
				Vec2 normal = difference / distance;
				auto bullet = m_entityManager.addEntity("Bullet");
				bullet->addComponent<CTransform>(Vec2(source), Vec2(m_fragConfig.SPEED * normal.x, m_fragConfig.SPEED * normal.y),
					Vec2(1, 1), -atan2(normal.x, normal.y) * 180 / 3.14159);
				bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("L_bullet"), true);
				bullet->addComponent<CBoundingBox>(Vec2(m_fragConfig.CX, m_fragConfig.CY), false, false);
				bullet->addComponent<CLifeSpan>(m_fragConfig.LIFESPAN);
				bullet->addComponent<CDamage>(m_fragConfig.DAMAGE);
			}
			e->destroy();
		}
	}
}

// Check for collisions between entities
void GameState_Play::sCollision()
{
	// Check for collisions with tiles
	for (auto e : m_entityManager.getEntities("Tile"))
	{
		if (!e->getComponent<CBoundingBox>()->blockMove)
		{
			continue;
		}
		// Check for player collisions with tiles
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			Vec2 prevOverlap = Physics::GetPreviousOverlap(e, m_player);
			// Player colliding from the side
			if (prevOverlap.x <= 0 && prevOverlap.y > 0)
			{
				// Colliding from the left
				if (m_player->getComponent<CTransform>()->pos.x < e->getComponent<CTransform>()->pos.x)
				{
					m_player->getComponent<CTransform>()->pos.x -= playerOverlap.x;
				}
				// Colliding from the right
				else
				{
					m_player->getComponent<CTransform>()->pos.x += playerOverlap.x;
				}
			}
			// Player colliding from the top or bottom
			else
			{
				// Colliding from the top
				if (m_player->getComponent<CTransform>()->pos.y < e->getComponent<CTransform>()->pos.y)
				{
					m_player->getComponent<CTransform>()->pos.y -= playerOverlap.y;
				}
				// Colliding from the bottom
				else
				{
					m_player->getComponent<CTransform>()->pos.y += playerOverlap.y;
				}
			}
		}
		// Check for NPC collisions with tiles
		for (auto npc : m_entityManager.getEntities("NPC"))
		{
			Vec2 npcOverlap = Physics::GetOverlap(e, npc);
			if (npcOverlap.x > 0 && npcOverlap.y > 0)
			{
				Vec2 prevOverlap = Physics::GetPreviousOverlap(e, npc);
				// NPC colliding from the side
				if (prevOverlap.x <= 0 && prevOverlap.y > 0)
				{
					// Colliding from the left 
					if (npc->getComponent<CTransform>()->pos.x < e->getComponent<CTransform>()->pos.x)
					{
						npc->getComponent<CTransform>()->pos.x -= npcOverlap.x;
					}
					// Colliding from the right
					else
					{
						npc->getComponent<CTransform>()->pos.x += npcOverlap.x;
					}
				}
				// NPC colliding from the top or bottom
				else
				{
					// Colliding from the top
					if (npc->getComponent<CTransform>()->pos.y < e->getComponent<CTransform>()->pos.y)
					{
						npc->getComponent<CTransform>()->pos.y -= npcOverlap.y;
					}
					// Colliding from the bottom
					else
					{
						npc->getComponent<CTransform>()->pos.y += npcOverlap.y;
					}
				}
			}
		}
		// Check for bullet collisions with tiles
		for (auto bullet : m_entityManager.getEntities("Bullet"))
		{
			Vec2 bulletOverlap = Physics::GetOverlap(e, bullet);
			if (bulletOverlap.x > 0 && bulletOverlap.y > 0 && e->getComponent<CBoundingBox>()->blockVision)
			{
				bullet->destroy();
			}
		}
		// Check for grenade collision with tiles and bounce them off
		for (auto grenade : m_entityManager.getEntities("Grenade"))
		{
			Vec2 grenadeOverlap = Physics::GetOverlap(e, grenade);
			if (grenadeOverlap.x > 0 && grenadeOverlap.y > 0 && e->getComponent<CBoundingBox>()->blockVision)
			{
				Vec2 prevOverlap = Physics::GetPreviousOverlap(e, grenade);
				// Grenade colliding from the side
				if (prevOverlap.x <= 0 && prevOverlap.y > 0)
				{
					grenade->getComponent<CTransform>()->speed.x *= -1;
					// Colliding from the left 
					if (grenade->getComponent<CTransform>()->pos.x < e->getComponent<CTransform>()->pos.x)
					{
						grenade->getComponent<CTransform>()->pos.x -= grenadeOverlap.x;
					}
					// Colliding from the right
					else
					{
						grenade->getComponent<CTransform>()->pos.x += grenadeOverlap.x;
					}
				}
				// Grenade colliding from the top or bottom
				else
				{
					grenade->getComponent<CTransform>()->speed.y *= -1;
					// Colliding from the top
					if (grenade->getComponent<CTransform>()->pos.y < e->getComponent<CTransform>()->pos.y)
					{
						grenade->getComponent<CTransform>()->pos.y -= grenadeOverlap.y;
					}
					// Colliding from the bottom
					else
					{
						grenade->getComponent<CTransform>()->pos.y += grenadeOverlap.y;
					}
				}
			}
		}
	}

	// Check if the player has walked over any item or weapon pickups

	for (auto e : m_entityManager.getEntities("HealthPot"))
	{
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CInventory>()->hPotCount++;
			e->destroy();
		}

	}
	for (auto e : m_entityManager.getEntities("ShieldPot"))
	{
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CInventory>()->sPotCount++;
			e->destroy();
		}
	}
	for (auto e : m_entityManager.getEntities("SpeedPot"))
	{
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CInventory>()->speedCount++;
			e->destroy();
		}
	}
	for (auto e : m_entityManager.getEntities("StealthPot"))
	{
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CInventory>()->stealthCount++;
			e->destroy();
		}
	}
	for (auto e : m_entityManager.getEntities("Shotgun"))
	{
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CWeapons>()->hasTwo = true;
			m_game.getShotgun();
			e->destroy();
		}
	}
	for (auto e : m_entityManager.getEntities("Rifle"))
	{
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CWeapons>()->hasThree = true;
			m_game.getRifle();
			e->destroy();
		}
	}
	for (auto e : m_entityManager.getEntities("Launcher"))
	{
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CWeapons>()->hasFour = true;
			m_game.getLauncher();
			e->destroy();
		}
	}

	// Collisions with spikes
	for (auto e : m_entityManager.getEntities("Spike"))
	{
		if (!e->getComponent<CBoundingBox>()->blockMove)
		{
			continue;
		}
		// Check for player collisions with tiles
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			inflictDamage(e, m_player);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(e, m_player);
			// Player colliding from the side
			if (prevOverlap.x <= 0 && prevOverlap.y > 0)
			{
				// Colliding from the left
				if (m_player->getComponent<CTransform>()->pos.x < e->getComponent<CTransform>()->pos.x)
				{
					m_player->getComponent<CTransform>()->pos.x -= playerOverlap.x;
				}
				// Colliding from the right
				else
				{
					m_player->getComponent<CTransform>()->pos.x += playerOverlap.x;
				}
			}
			// Player colliding from the top or bottom
			else
			{
				// Colliding from the top
				if (m_player->getComponent<CTransform>()->pos.y < e->getComponent<CTransform>()->pos.y)
				{
					m_player->getComponent<CTransform>()->pos.y -= playerOverlap.y;
				}
				// Colliding from the bottom
				else
				{
					m_player->getComponent<CTransform>()->pos.y += playerOverlap.y;
				}
			}
		}
		// Check for NPC collisions with tiles
		for (auto npc : m_entityManager.getEntities("NPC"))
		{
			Vec2 npcOverlap = Physics::GetOverlap(e, npc);
			if (npcOverlap.x > 0 && npcOverlap.y > 0)
			{
				inflictDamage(e, npc);
				Vec2 prevOverlap = Physics::GetPreviousOverlap(e, npc);
				// NPC colliding from the side
				if (prevOverlap.x <= 0 && prevOverlap.y > 0)
				{
					// Colliding from the left 
					if (npc->getComponent<CTransform>()->pos.x < e->getComponent<CTransform>()->pos.x)
					{
						npc->getComponent<CTransform>()->pos.x -= npcOverlap.x;
					}
					// Colliding from the right
					else
					{
						npc->getComponent<CTransform>()->pos.x += npcOverlap.x;
					}
				}
				// NPC colliding from the top or bottom
				else
				{
					// Colliding from the top
					if (npc->getComponent<CTransform>()->pos.y < e->getComponent<CTransform>()->pos.y)
					{
						npc->getComponent<CTransform>()->pos.y -= npcOverlap.y;
					}
					// Colliding from the bottom
					else
					{
						npc->getComponent<CTransform>()->pos.y += npcOverlap.y;
					}
				}
			}
		}
		// Check for bullet collisions with tiles
		for (auto bullet : m_entityManager.getEntities("Bullet"))
		{
			Vec2 bulletOverlap = Physics::GetOverlap(e, bullet);
			if (bulletOverlap.x > 0 && bulletOverlap.y > 0 && e->getComponent<CBoundingBox>()->blockVision)
			{
				bullet->destroy();
			}
		}
		for (auto grenade : m_entityManager.getEntities("Grenade"))
		{
			Vec2 grenadeOverlap = Physics::GetOverlap(e, grenade);
			if (grenadeOverlap.x > 0 && grenadeOverlap.y > 0 && e->getComponent<CBoundingBox>()->blockVision)
			{
				Vec2 prevOverlap = Physics::GetPreviousOverlap(e, grenade);
				// Grenade colliding from the side
				if (prevOverlap.x <= 0 && prevOverlap.y > 0)
				{
					grenade->getComponent<CTransform>()->speed.x *= -1;
					// Colliding from the left 
					if (grenade->getComponent<CTransform>()->pos.x < e->getComponent<CTransform>()->pos.x)
					{
						grenade->getComponent<CTransform>()->pos.x -= grenadeOverlap.x;
					}
					// Colliding from the right
					else
					{
						grenade->getComponent<CTransform>()->pos.x += grenadeOverlap.x;
					}
				}
				// Grenade colliding from the top or bottom
				else
				{
					grenade->getComponent<CTransform>()->speed.y *= -1;
					// Colliding from the top
					if (grenade->getComponent<CTransform>()->pos.y < e->getComponent<CTransform>()->pos.y)
					{
						grenade->getComponent<CTransform>()->pos.y -= grenadeOverlap.y;
					}
					// Colliding from the bottom
					else
					{
						grenade->getComponent<CTransform>()->pos.y += grenadeOverlap.y;
					}
				}
			}
		}
	}

	// Check for collisions with NPCs
	for (auto e : m_entityManager.getEntities("NPC"))
	{

		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			inflictDamage(e, m_player);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(e, m_player);
			// Player colliding from the side
			if (prevOverlap.x <= 0 && prevOverlap.y > 0)
			{
				// Colliding from the left
				if (m_player->getComponent<CTransform>()->pos.x < e->getComponent<CTransform>()->pos.x)
				{
					m_player->getComponent<CTransform>()->pos.x -= playerOverlap.x+50;
				}
				// Colliding from the right
				else
				{
					m_player->getComponent<CTransform>()->pos.x += playerOverlap.x+50;
				}
			}
			// Player colliding from the top or bottom
			else
			{
				// Colliding from the top
				if (m_player->getComponent<CTransform>()->pos.y < e->getComponent<CTransform>()->pos.y)
				{
					m_player->getComponent<CTransform>()->pos.y -= playerOverlap.y+50;
				}
				// Colliding from the bottom
				else
				{
					m_player->getComponent<CTransform>()->pos.y += playerOverlap.y+50;
				}
			}
		}


		for (auto bullet : m_entityManager.getEntities("Bullet"))
		{
			Vec2 bulletOverlap = Physics::GetOverlap(e, bullet);
			if (bulletOverlap.x > 0 && bulletOverlap.y > 0)
			{
				inflictDamage(bullet, e);
				if (e->getComponent<CHealth>()->currentHP <= 0)
				{
					// Instantiate an explosion animation before destroying the NPC entity
					auto explosion = m_entityManager.addEntity("explosion");
					explosion->addComponent<CTransform>(Vec2(e->getComponent<CTransform>()->pos));
					explosion->addComponent<CBoundingBox>(Vec2(1, 1), false, false);
					explosion->addComponent<CAnimation>(m_game.getAssets().getAnimation("Explosion"), false);
					explosion->addComponent<CLifeSpan>(2000);
				}
				bullet->destroy();
			}
		}
	}
}

// Assign and run animations
void GameState_Play::sAnimation()
{
	for (auto e : m_entityManager.getEntities())
	{
		// Update all animations and destroy entities if their animation ends without looping
		e->getComponent<CAnimation>()->animation.update();
		if (e->getComponent<CAnimation>()->animation.hasEnded() && !(e->getComponent<CAnimation>()->repeat))
		{
			e->destroy();
		}
		else if (e->getComponent<CAnimation>()->animation.getName() == "PlayerA" && e->getComponent<CAnimation>()->animation.hasEnded() && m_player->getComponent<CAnimation>()->animation.getName() != "PlayerDeath")
		{
			m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("PlayerM"), true);
		}
	}

	auto color = m_player->getComponent<CAura>()->color;

	switch (color)
	{
	case 1: {player_aura->addComponent<CAnimation>(m_game.getAssets().getAnimation("NoAura"), true); break; }
	case 2: {player_aura->addComponent<CAnimation>(m_game.getAssets().getAnimation("RedAura"), true); break; }
	case 3: {player_aura->addComponent<CAnimation>(m_game.getAssets().getAnimation("GreenAura"), true); break; }
	case 4: {player_aura->addComponent<CAnimation>(m_game.getAssets().getAnimation("BlueAura"), true); break; }
	}
}

// Receive user input
void GameState_Play::sUserInput()
{
    auto pInput = m_player->getComponent<CInput>();

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		Vec2 mousePosition = Vec2(sf::Mouse::getPosition(m_game.window()).x - m_windowX / 2, sf::Mouse::getPosition(m_game.window()).y - m_windowY / 2);
		Vec2 target = mousePosition + Vec2(m_player->getComponent<CTransform>()->pos);
		fireWeapon(m_player, target);
		m_player->getComponent<CWeapons>()->canShoot = false;
	}

    sf::Event event;
    while (m_game.window().pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_game.quit();
        }
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:  { m_game.popState(); break; }
                case sf::Keyboard::W:       { pInput->up = true; break; }
                case sf::Keyboard::A:       { pInput->left = true; break; }
                case sf::Keyboard::S:       { pInput->down = true; break; }
                case sf::Keyboard::D:       { pInput->right = true; break; }
                case sf::Keyboard::Z:       { init(m_levelPath); break; }
                case sf::Keyboard::T:       { m_drawTextures = !m_drawTextures; break; }
                case sf::Keyboard::F:       { m_drawCollision = !m_drawCollision; break; }
                case sf::Keyboard::P:       { setPaused(!m_paused); break; }
				case sf::Keyboard::R:		{startreload(); break; }
				case sf::Keyboard::LShift:
				{
					auto dash = m_player->getComponent<CDash>();
					if (dash->canDash)
					{
						dash->isDashing = true;
						dash->canDash = false;
						dash->dashTimer = dash->clock.getElapsedTime().asMilliseconds() + dash->dashDuration;
						m_player->getComponent<CTransform>()->speed *= dash->dashSpeed;
					}
					break;
				}
				case sf::Keyboard::Tab: { m_showInventory = !m_showInventory; setPaused(!m_paused); break; }
				case sf::Keyboard::J:
				{
					Inventory::UseHealthPotion(m_player);
					break;
				}
				case sf::Keyboard::K:
				{
					Inventory::UseShieldPotion(m_player);
					break;
				}
				case sf::Keyboard::U:
				{
					Inventory::UseSpeedPotion(m_player);
					break;
				}
				case sf::Keyboard::I:
				{
					Inventory::UseStealthPotion(m_player);
					break;
				}
				case sf::Keyboard::Num1:	
				{
					m_player->getComponent<CAura>()->color = 1;
					m_player->getComponent<CWeapons>()->current = 1; 
					break;
				}
				case sf::Keyboard::Num2: 
				{

					if (m_player->getComponent<CWeapons>()->hasTwo)
					{
						m_player->getComponent<CAura>()->color = 2;
						m_player->getComponent<CWeapons>()->current = 2;
					}
					break;
				}
				case sf::Keyboard::Num3:
				{

					if (m_player->getComponent<CWeapons>()->hasThree)
					{
						m_player->getComponent<CAura>()->color = 3;
						m_player->getComponent<CWeapons>()->current = 3;
					}
					break;
				}
				case sf::Keyboard::Num4:
				{
					if (m_player->getComponent<CWeapons>()->hasFour)
					{
						m_player->getComponent<CAura>()->color = 4;
						m_player->getComponent<CWeapons>()->current = 4;
					}
					break;
				}
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:       { pInput->up = false; break; }
                case sf::Keyboard::A:       { pInput->left = false; break; }
                case sf::Keyboard::S:       { pInput->down = false; break; }
                case sf::Keyboard::D:       { pInput->right = false; break; }
            }
        }

		if (event.type == sf::Event::MouseButtonReleased && !m_paused)
		{
			m_player->getComponent<CWeapons>()->canShoot = true;
		}
    }
}

// Determine camera view and draw all entities to the screen
void GameState_Play::sRender()
{
	m_game.window().clear(sf::Color(255, 192, 122));
	sf::View view = m_game.window().getView();

	// Center camera on the player
	view.setCenter(sf::Vector2f(m_player->getComponent<CTransform>()->pos.x, m_player->getComponent<CTransform>()->pos.y));
	m_game.window().setView(view);

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			auto transform = e->getComponent<CTransform>();

			if (e->hasComponent<CAnimation>())
			{
				auto animation = e->getComponent<CAnimation>()->animation;
				animation.getSprite().setRotation(transform->angle);
				animation.getSprite().setPosition(transform->pos.x, transform->pos.y);
				animation.getSprite().setScale(transform->scale.x, transform->scale.y);
				m_game.window().draw(animation.getSprite());
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto box = e->getComponent<CBoundingBox>();
				auto transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box->size.x - 1, box->size.y - 1));
				rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
				rect.setPosition(transform->pos.x, transform->pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (box->blockMove && box->blockVision) { rect.setOutlineColor(sf::Color::Black); }
				if (box->blockMove && !box->blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!box->blockMove && box->blockVision) { rect.setOutlineColor(sf::Color::Red); }
				if (!box->blockMove && !box->blockVision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game.window().draw(rect);
			}

			if (e->hasComponent<CPatrol>())
			{
				auto & patrol = e->getComponent<CPatrol>()->positions;
				for (size_t p = 0; p < patrol.size(); p++)
				{
					dot.setPosition(patrol[p].x, patrol[p].y);
					m_game.window().draw(dot);
				}
			}

			if (e->hasComponent<CFollowPlayer>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x = e->getComponent<CTransform>()->pos.x;
				lines[0].position.y = e->getComponent<CTransform>()->pos.y;
				lines[0].color = sf::Color::Black;
				lines[1].position.x = m_player->getComponent<CTransform>()->pos.x;
				lines[1].position.y = m_player->getComponent<CTransform>()->pos.y;
				lines[1].color = sf::Color::Black;
				m_game.window().draw(lines);
				dot.setPosition(e->getComponent<CFollowPlayer>()->home.x, e->getComponent<CFollowPlayer>()->home.y);
				m_game.window().draw(dot);
			}
		}
	}

	if (m_showInventory)
	{
		auto window = m_game.window().getSize();
		auto pPos = m_player->getComponent<CTransform>();
		auto health = m_entityManager.getEntities("HealthPot");

		m_itemLabel.setPosition(pPos->pos.x + 270, pPos->pos.y - 350);
		m_itemLabel.setString("ITEMS");
		m_itemLabel.setCharacterSize(48);

		m_weaponLabel.setPosition(pPos->pos.x + 270, pPos->pos.y - 51);
		m_weaponLabel.setString("WEAPONS");
		m_weaponLabel.setCharacterSize(48);

		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(window.x * 0.3 - 5, window.y - 65));
		rect.setPosition(pPos->pos.x + 255, pPos->pos.y - 380);
		rect.setFillColor(sf::Color::Black);
		rect.setOutlineThickness(5);

		sf::RectangleShape healthRect;
		healthRect.setSize(sf::Vector2f(64, 64));
		//healthRect.setTexture(m_game.getAssets().getAnimation("HPot").getSprite());
		//healthRect.setTexture((sf::Texture)health->getComponent<CAnimation>()->animation.getName());
		healthRect.setPosition(pPos->pos.x + 320, pPos->pos.y - 270);

		sf::RectangleShape shieldRect;
		shieldRect.setSize(sf::Vector2f(64, 64));
		//shieldRect.setTexture();
		shieldRect.setPosition(pPos->pos.x + 484, pPos->pos.y - 270);

		sf::RectangleShape speedRect;
		speedRect.setSize(sf::Vector2f(64, 64));
		speedRect.setPosition(pPos->pos.x + 320, pPos->pos.y - 156);

		sf::RectangleShape stealthRect;
		stealthRect.setSize(sf::Vector2f(64, 64));
		stealthRect.setPosition(pPos->pos.x + 484, pPos->pos.y - 156);

		sf::RectangleShape w1Rect;
		w1Rect.setSize(sf::Vector2f(64, 64));
		//shieldRect.setTexture();
		w1Rect.setPosition(pPos->pos.x + 320, pPos->pos.y + 60);

		sf::RectangleShape w2Rect;
		w2Rect.setSize(sf::Vector2f(64, 64));
		//shieldRect.setTexture();
		w2Rect.setPosition(pPos->pos.x + 484, pPos->pos.y + 60);

		sf::RectangleShape w3Rect;
		w3Rect.setSize(sf::Vector2f(64, 64));
		//shieldRect.setTexture();
		w3Rect.setPosition(pPos->pos.x + 320, pPos->pos.y + 174);

		sf::RectangleShape w4Rect;
		w4Rect.setSize(sf::Vector2f(64, 64));
		//shieldRect.setTexture();
		w4Rect.setPosition(pPos->pos.x + 484, pPos->pos.y + 174);

		m_healthCount.setPosition(pPos->pos.x + 384, pPos->pos.y - 206);
		m_healthCount.setCharacterSize(12);
		m_healthCount.setString("x" + std::to_string(m_player->getComponent<CInventory>()->hPotCount));

		m_shieldCount.setPosition(pPos->pos.x + 548, pPos->pos.y - 206);
		m_shieldCount.setCharacterSize(12);
		m_shieldCount.setString("x" + std::to_string(m_player->getComponent<CInventory>()->sPotCount));

		m_speedCount.setPosition(pPos->pos.x + 384, pPos->pos.y - 92);
		m_speedCount.setCharacterSize(12);
		m_speedCount.setString("x" + std::to_string(m_player->getComponent<CInventory>()->speedCount));


		m_stealthCount.setPosition(pPos->pos.x + 548, pPos->pos.y - 92);
		m_stealthCount.setCharacterSize(12);
		m_stealthCount.setString("x" + std::to_string(m_player->getComponent<CInventory>()->stealthCount));

		m_game.window().draw(rect);
		m_game.window().draw(healthRect);
		m_game.window().draw(shieldRect);
		m_game.window().draw(speedRect);
		m_game.window().draw(stealthRect);
		m_game.window().draw(w1Rect);
		m_game.window().draw(w2Rect);
		m_game.window().draw(w3Rect);
		m_game.window().draw(w4Rect);
		m_game.window().draw(m_itemLabel);
		m_game.window().draw(m_weaponLabel);
		m_game.window().draw(m_healthCount);
		m_game.window().draw(m_shieldCount);
		m_game.window().draw(m_speedCount);
		m_game.window().draw(m_stealthCount);
	}

	m_playerHealth.setPosition(m_player->getComponent<CTransform>()->pos.x - 600, m_player->getComponent<CTransform>()->pos.y + 250);
	m_playerHealth.setString(std::to_string(m_player->getComponent<CHealth>()->currentHP));
	m_game.window().draw(m_playerHealth);

	m_playerShields.setPosition(m_player->getComponent<CTransform>()->pos.x - 500, m_player->getComponent<CTransform>()->pos.y + 250);
	m_playerShields.setString(std::to_string(m_player->getComponent<CHealth>()->currentShield));
	m_game.window().draw(m_playerShields);

	m_playerAmmo.setPosition(m_player->getComponent<CTransform>()->pos.x - 400, m_player->getComponent<CTransform>()->pos.y + 250);
	switch (m_player->getComponent<CWeapons>()->current)
	{
	case 1:
	{
		m_playerAmmo.setString(std::to_string(m_player->getComponent<CWeapons>()->pistol_ammo));
		break;
	}
	case 2:
	{
		m_playerAmmo.setString(std::to_string(m_player->getComponent<CWeapons>()->shotgun_ammo));
		break;
	}
	case 3:
	{
		m_playerAmmo.setString(std::to_string(m_player->getComponent<CWeapons>()->rifle_ammo));
		break;
	}
	case 4:
	{
		m_playerAmmo.setString(std::to_string(m_player->getComponent<CWeapons>()->launcher_ammo));
		break;
	}
	}
	m_game.window().draw(m_playerAmmo);

	m_game.window().display();
}

