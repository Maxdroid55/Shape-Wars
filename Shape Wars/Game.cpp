#include "Game.h"
#include <fstream>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float randomNum(float min, float max) { return min + std::fmod(rand(), 1 + max - min); }

int randomNum(int min, int max) { return min + std::fmod(rand(), 1 + max - min);}

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	// TODO: read in config file here 
	//		use PlayerConfig, EnemyConfig, BulletConfig variables
	std::ifstream fin(config);
	std::string object;

	while (fin >> object)
	{
		if (object == "Window")
		{
			int width, height;
			bool isFullScreen;

			fin >> width >> height >> m_framerateLimit >> isFullScreen;
			if (!isFullScreen)
			{
				m_window.create(sf::VideoMode(width, height), "Shape Wars");
			}
			else
			{
				m_window.create(sf::VideoMode(width, height), "Shape Wars", sf::Style::Fullscreen);
			}

			m_window.setFramerateLimit(m_framerateLimit);
		}
		else if (object == "Font")
		{
			std::string fontPath;
			int fontSize, r, g, b;

			fin >> fontPath >> fontSize >> r >> g >> b;

			// load a font
			if (!m_font.loadFromFile(fontPath))
			{
				//std::cout << "failed to load font!..." << std::endl;
			}

			m_text.setFont(m_font);
			m_text.setCharacterSize(fontSize);
			m_text.setFillColor(sf::Color(r, g ,b));
			m_text.setPosition(15, 15);
		}
		else if (object == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
				>> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		}
		else if (object == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG 
				>> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}
		else if (object == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >>  m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR
				>> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}
	spawnPlayer();
}

void Game::spawnPlayer()
{
	 //We create every entity by calling EntityManager.addEntity(tag)
	 //This returns a std::shared_ptr<Entity> so use 'auto' to save typing.
	auto entity = m_entities.addEntity("player");

	// Give this entity(player) a Transfom so it spawns at center of the window with velocity (1, 1) and angle 0
	entity->cTransform = std::make_shared<CTransform>(
		Vec2(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f),
		Vec2(1.0f, 1.0f), 
		0.0f
	);

	// The entity's shape will have a radius 32, 8 sides, dark grey fill, and red outline of thickness 4
	entity->cShape = std::make_shared<CShape>(
		m_playerConfig.SR, 
		m_playerConfig.V, 
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT
	);

	// Add an input component to the player so that we can use inputs
	entity->cInput = std::make_shared<CInput>();

	// Since we want this Entity to be our player, set our Game's player varible to be this entity.
	// We do this because we use the player so much in the logic of our game class, it make this easier,
	// even though it does not conform to the entity manager paradigm (factory pattern)
	m_player = entity;
}

void Game::run()
{
	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sEnemySpawner();
			sCollions();
			sMovement();
			m_currentFrame++;
		}

		sUserInput();
		sRender();
	}

}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::sMovement()
{
	m_player->cTransform->velocity = { 0, 0 };
	float playerRadius = static_cast<float>(m_playerConfig.CR + 10);

	// implement player movement
	if (m_player->cInput->up &&
		m_player->cTransform->pos.y > playerRadius)
	{
		m_player->cTransform->velocity.y = -m_playerConfig.S;
	}

	if (m_player->cInput->down &&
		m_player->cTransform->pos.y < m_window.getSize().y - playerRadius)
	{
		m_player->cTransform->velocity.y = m_playerConfig.S;
	}	
	
	if (m_player->cInput->left &&
		m_player->cTransform->pos.x > playerRadius)
	{
		m_player->cTransform->velocity.x = -m_playerConfig.S;
	}	
	
	if (m_player->cInput->right &&
		m_player->cTransform->pos.x < m_window.getSize().x - playerRadius)
	{
		m_player->cTransform->velocity.x = m_playerConfig.S;
	}

	// movement of entities
	for (auto& entity : m_entities.getEntities())
	{
		entity->cTransform->pos.x += entity->cTransform->velocity.x;
		entity->cTransform->pos.y += entity->cTransform->velocity.y;	
	}
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// this event triggers when the window is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
			m_window.close();
		}

		// this event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up	= true;
				break;

			case sf::Keyboard::A:
				m_player->cInput->left  = true;
				break;

			case sf::Keyboard::S:
				m_player->cInput->down	= true;
				break;

			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;

			case sf::Keyboard::P:
				setPaused(!m_paused);
				break;

			case sf::Keyboard::Escape:
				m_running = false;
				m_window.close();
				break;

			default:
				break;
			}
		}

		// this event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;

			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;

			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;

			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;

			default:
				break;
			}
		}
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				// call spawnBullet here
				spawnBullet(m_player, Vec2(event.mouseButton.x / 1.0f, event.mouseButton.y / 1.0f));
			}
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				// call spawnSpecialWeapon here
				spawnSpecialWeapon(m_player);
			}
		}

		if (event.type == sf::Event::Resized)
		{
			m_window.create(sf::VideoMode(event.size.width, event.size.height), "Shape Wars");
			m_window.setFramerateLimit(m_framerateLimit);
		}
	}
}

void Game::sRender()
{
	m_window.clear();

	for (auto e : m_entities.getEntities())
	{
		// set the position of the shape based on the entity's transform->pos
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		// set the rotation of the shape based on the entity's transform->angle
		e->cTransform->angle += 4.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		// change lifespan
		if (e->cLifespan)
		{
			if (e->cLifespan->remaining > 0)
			{
				int opacity = static_cast<int>((static_cast<float>(e->cLifespan->remaining) / static_cast<float>(e->cLifespan->total)) * 255.0f);
				int r = e->cShape->circle.getFillColor().r;
				int g = e->cShape->circle.getFillColor().g;
				int b = e->cShape->circle.getFillColor().b;
				e->cShape->circle.setFillColor(sf::Color(r, g, b, opacity));
				e->cShape->circle.setOutlineColor(sf::Color(255, 255, 255, opacity));
				e->cLifespan->remaining--;
			}
			else
			{
				// destroy an entity if it's lifespan runs out
				e->destroy();
			}
		}

		m_text.setString("SCORE: " + std::to_string(m_score));
		m_window.draw(m_text);

		//draw the entity's sf::CircleShape
		m_window.draw(e->cShape->circle);
	}

	m_window.display();
}

void Game::sEnemySpawner()
{
	if (m_currentFrame % m_enemyConfig.SI ==  0)
	{
		spawnEnemy();
	}
}

void Game::sCollions()
{
	// Handle collision between an enemy and the game window
	// When an enemy collides with the window 'bounces' of the window in the opposite direction
	for (auto& enemy : m_entities.getEntities("enemy"))
	{
		const float ER = static_cast<float>(m_enemyConfig.CR);
		float r = ER;
		const float WIDTH = static_cast<float>(m_window.getSize().x);
		const float HEIGHT = static_cast<float>(m_window.getSize().y);

		// check if enemy collides with left  or right bound of the window
		if (enemy->cTransform->pos.x + r >= WIDTH || enemy->cTransform->pos.x <= r)
		{
			enemy->cTransform->velocity.x *= -1.0;
		}

		// check if enemy collides with the top or bottom bound of the window
		if (enemy->cTransform->pos.y + r >= HEIGHT || enemy->cTransform->pos.y <= r)
		{
			enemy->cTransform->velocity.y *= -1.0;
		}


		// Handle collision between an enemy and a bullet
		// D = enemy - bullet
		// the bullet and enemy have collided if :
		// D.x * D.x + D.y * D.y < (radiusBullet + radiusEnemy) * (radiusBullet + radiusEnemy)
		// we could have used sqrt() function but it is expensive and we don't need an actual pin point accurate distance, we need a close enough approximate
		// This is an optimization
		for (auto& bullet : m_entities.getEntities("bullet"))
		{
			Vec2 D = enemy->cTransform->pos - bullet->cTransform->pos;
			const float BR = bullet->cShape->circle.getRadius();

			if (D.x * D.x + D.y * D.y < (BR + ER) * (BR + ER))
			{
				if (enemy->cScore)
				{
					m_score += enemy->cScore->score;
				}
				bullet->destroy();
				enemy->destroy();
				if (enemy->cShape->circle.getRadius() >= m_enemyConfig.SR)
				{
					spawnSmallEnemies(enemy);
				}
			}
		}

		// Handle collision between enemy and player
		Vec2 D = enemy->cTransform->pos - m_player ->cTransform->pos;
		const float PR = static_cast<float>(m_playerConfig.CR);

		if (D.x * D.x + D.y * D.y <= (PR + ER) * (PR + ER))
		{
			m_score = std::max(0, m_score - 100);
			m_player->destroy();
			enemy->destroy();
			spawnPlayer();
			if (static_cast<int>(enemy->cShape->circle.getRadius()) == m_enemyConfig.SR)
			{
				spawnSmallEnemies(enemy);
			}
		}
	}
}

void Game::spawnEnemy()
{
	auto entity = m_entities.addEntity("enemy");

	int numSides = randomNum(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
	float radius = static_cast<float>(m_enemyConfig.SR);

	float x = randomNum(m_enemyConfig.CR, m_window.getSize().x - m_enemyConfig.CR);
	float y = randomNum(m_enemyConfig.CR, m_window.getSize().y - m_enemyConfig.CR);

	// generate random color
	int r = randomNum(0, 255);
	int g = randomNum(0, 255);
	int b = randomNum(0, 255);

	float speed = -m_enemyConfig.SMIN + (rand() % 1 + m_enemyConfig.SMAX - m_enemyConfig.SMIN);

	entity->cTransform = std::make_shared<CTransform>(
		Vec2(x, y), 
		Vec2(speed, speed),
		0.0f
	);

	entity->cShape = std::make_shared<CShape>(radius,
		numSides, 
		sf::Color(r, g, b), 
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
		m_enemyConfig.OT
	);

	entity->cScore = std::make_shared<CScore>(numSides*100);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
	size_t numOfPoints = entity->cShape->circle.getPointCount();
	float r = m_enemyConfig.SR / 2.0f;
	sf::Color c = entity->cShape->circle.getFillColor();
	Vec2 pos = entity->cTransform->pos;

	// Calculate the speed and the angle step
	float speed = 4.0f;
	float angleStep = 360.0f / static_cast<float>(numOfPoints);

	for (size_t i = 0; i < numOfPoints; i++)
	{
		auto enemy = m_entities.addEntity("enemy");

		float angle = angleStep * i;
		float angleRad = angle * ( M_PI / 180.0f); // Convert angle to radians

		// Calculate velocity components based on the angle
		float vx = speed * cos(angleRad);
		float vy = speed * sin(angleRad);

		enemy->cTransform = std::make_shared<CTransform>(pos, Vec2(vx, vy), 0.0f);
		enemy->cShape = std::make_shared<CShape>(r, static_cast<int>(numOfPoints), c, sf::Color(255, 255, 255), m_enemyConfig.OT);
		enemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
		enemy->cScore = std::make_shared<CScore>(static_cast<int>(numOfPoints) * 200);

		m_lastEnemySpawnTime = m_currentFrame;
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos)
{
	auto bullet = m_entities.addEntity("bullet");

	// calculate the distance between the player and the mouse position (vectors)
	Vec2 distance = Vec2(m_player->cTransform->pos.x - mousePos.x, m_player->cTransform->pos.y - mousePos.y);

	// this will use the angle to calculate the velocity of the bullet
	// calculate the angle "between" the player and the mouse
	// this angle will be used to calculate the velocity of the bullet to ensure it travels to the direction that the mouse was clicked.
	//float angle = atan2f(distance.y, distance.x);

	// We can also use normalize the vector by diving x and y of the distance by L
	// Meaning we are making it's length to be one while maintaining the original angle
	float L = sqrtf(distance.x * distance.x + distance.y * distance.y);

	// The speed is negative to account for the fact that the 2d plane of the SFML library is kinda of inverted from the normal cartisian plane.
	// y increases from top to bottom and x increases from left to right.
	// if it the speed was positive it would shoot in the opposite direction of the mouse position.
	float speed = -m_bulletConfig.S;
	Vec2 playerPos(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
	Vec2 bulletVelocity(speed * (distance.x / L), speed * (distance.y / L));

	//bullet->cTransform = std::make_shared<CTransform>(Vec2(m_player->cTransform->pos.x, m_player->cTransform->pos.y), Vec2(speed * cos(angle), speed * sin(angle)), 0.0f);
	bullet->cTransform = std::make_shared<CTransform>(playerPos, bulletVelocity, m_bulletConfig.OT);
	bullet->cShape = std::make_shared<CShape>(
		m_bulletConfig.SR, 
		m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), 
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), 
		m_bulletConfig.OT
	);
	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	size_t numOfPoints = m_playerConfig.V * 4;
	float r = m_bulletConfig.SR;
	sf::Color fc = entity->cShape->circle.getFillColor();
	sf::Color oc = entity->cShape->circle.getOutlineColor();
	Vec2 pos = entity->cTransform->pos;

	// Calculate the speed and the angle step
	float speed = 4.0f;
	float angleStep = 360.0f / static_cast<float>(numOfPoints);

	for (size_t i = 0; i < numOfPoints; i++)
	{
		auto b = m_entities.addEntity("bullet");

		float angle = angleStep * i;
		float angleRad = angle * (M_PI / 180.0f); // Convert angle to radians

		// Calculate velocity components based on the angle
		float vx = speed * cos(angleRad);
		float vy = speed * sin(angleRad);

		b->cTransform = std::make_shared<CTransform>(pos, Vec2(vx, vy), 0.0f);
		b->cShape = std::make_shared<CShape>(
			r, 
			static_cast<int>(numOfPoints), 
			fc,
			oc,
			entity->cShape->circle.getOutlineThickness()
		);
		std::cout << b->cShape->circle.getOutlineColor().r << " " << b->cShape->circle.getOutlineColor().g << " " << b->cShape->circle.getOutlineColor().b << std::endl;
		b->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
	}
}

