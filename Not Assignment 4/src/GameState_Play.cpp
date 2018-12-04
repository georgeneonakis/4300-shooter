#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

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

		else if (start == "NPC")
		{
			auto npc = m_entityManager.addEntity("NPC");
			std::string name, aiType;
			float rx, ry, tx, ty, bm, bv, speed;
			fin >> name >> rx >> ry >> tx >> ty >> bm >> bv >> aiType >> speed;

			npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(name), true);
			npc->addComponent<CTransform>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64));
			npc->addComponent<CBoundingBox>(npc->getComponent<CAnimation>()->animation.getSize(), bm, bv);
			if (aiType == "Follow")
			{
				npc->addComponent<CFollowPlayer>(Vec2(rx * m_windowX + tx * 64, ry * m_windowY + ty * 64), speed);
			}
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

		else if (start == "Player")
		{
			fin >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED;
		}

		else if (start == "Bullet")
		{
			fin >> m_bulletConfig.CX >> m_bulletConfig.CY >> m_bulletConfig.SPEED >> m_bulletConfig.LIFESPAN;
		}
		getline(fin, empty, '\n');
	}
    spawnPlayer();
}

// Spawn the player
void GameState_Play::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("StandDown"), true);
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), false, false);
    m_player->addComponent<CInput>();
}

// Fire the current weapon at the cursor
void GameState_Play::fireWeapon(std::shared_ptr<Entity> entity, const Vec2 & target)
{
	// Calculate velocity of the bullet
	Vec2 difference = target - entity->getComponent<CTransform>()->pos;
	float distance = target.dist(entity->getComponent<CTransform>()->pos);
	difference /= distance;
	auto bullet = m_entityManager.addEntity("Bullet");
	// Assign a CTransform, CAnimation, CBoundingBox, and CLifespan component
	bullet->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>()->pos), Vec2(m_bulletConfig.SPEED * difference.x, m_bulletConfig.SPEED * difference.y), 
									 Vec2(1, 1), m_player->getComponent<CTransform>()->angle);
	bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("SwordRight"), true);
	bullet->addComponent<CBoundingBox>(Vec2(m_bulletConfig.CX, m_bulletConfig.CY), false, false);
	bullet->addComponent<CLifeSpan>(m_bulletConfig.LIFESPAN);
}

// Game loop
void GameState_Play::update()
{
    m_entityManager.update();

    if (!m_paused)
    {
        sAI();
        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
    }

    sUserInput();
    sRender();
}

// Move entities each frame
void GameState_Play::sMovement()
{
	auto pInput = m_player->getComponent<CInput>();
	auto pTransform = m_player->getComponent<CTransform>();
	pTransform->speed = Vec2(0, 0);
	// Determine player's speed based on input
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
	}
}

// Generate AI behavior
void GameState_Play::sAI()
{
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
			// If vision is not blocked, pursue player
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
	for (auto e : m_entityManager.getEntities("Bullet"))
	{
		int elap = e->getComponent<CLifeSpan>()->clock.getElapsedTime().asMilliseconds();
		int life = e->getComponent<CLifeSpan>()->lifespan;

		if (elap > life)
		{
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
	}
	// Check for collisions with NPCs
	for (auto e : m_entityManager.getEntities("NPC"))
	{
		// Respawn the player if they collide with an NPC
		Vec2 playerOverlap = Physics::GetOverlap(e, m_player);
		if (playerOverlap.x > 0 && playerOverlap.y > 0)
		{
			m_player->getComponent<CTransform>()->pos = Vec2(m_playerConfig.X, m_playerConfig.Y);
		}
		// Destroy the NPC if it collides with a bullet
		for (auto bullet : m_entityManager.getEntities("Bullet"))
		{
			Vec2 bulletOverlap = Physics::GetOverlap(e, bullet);
			if (bulletOverlap.x > 0 && bulletOverlap.y > 0)
			{
				// Instantiate an explosion animation before destroying the NPC entity
				auto explosion = m_entityManager.addEntity("explosion");
				explosion->addComponent<CTransform>(Vec2(e->getComponent<CTransform>()->pos));
				explosion->addComponent<CBoundingBox>(Vec2(1, 1), false, false);
				explosion->addComponent<CAnimation>(m_game.getAssets().getAnimation("Explosion"), false);
				explosion->addComponent<CLifeSpan>(2000);
				e->destroy();
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
	}
}

// Receive user input
void GameState_Play::sUserInput()
{
    auto pInput = m_player->getComponent<CInput>();

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
                case sf::Keyboard::R:       { m_drawTextures = !m_drawTextures; break; }
                case sf::Keyboard::F:       { m_drawCollision = !m_drawCollision; break; }
                case sf::Keyboard::P:       { setPaused(!m_paused); break; }
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

		if (event.type == sf::Event::MouseButtonPressed)
		{
			// Fire a bullet
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (m_player->getComponent<CInput>()->canShoot)
				{
					Vec2 mousePosition = Vec2(event.mouseButton.x - m_windowX / 2, event.mouseButton.y - m_windowY / 2);
					Vec2 target = mousePosition + Vec2(m_player->getComponent<CTransform>()->pos);
					fireWeapon(m_player, target);
					m_player->getComponent<CInput>()->canShoot = false;
				}
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			m_player->getComponent<CInput>()->canShoot = true;
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
                rect.setSize(sf::Vector2f(box->size.x-1, box->size.y-1));
                rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
                rect.setPosition(transform->pos.x, transform->pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box->blockMove && box->blockVision)  { rect.setOutlineColor(sf::Color::Black); }
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

    m_game.window().display();
}
