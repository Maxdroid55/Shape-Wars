#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>

struct CTransform
{
	Vec2 pos		= { 0.0, 0.0 };
	Vec2 velocity   = { 0.0, 0.0 };
	float angle		=  0;

	CTransform(const Vec2& p, const Vec2& v, float a)
		: pos(p), velocity(v), angle(a) {}
};

struct CShape
{
	sf::CircleShape circle;
	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

struct CCollision
{
	float radius = 0;
	CCollision(float r)
		: radius(r) {}
};

struct CInput
{
	bool up		= false;
	bool left	= false;
	bool right  = false;
	bool down	= false;
	bool shoot  = false;

	CInput() {}
};

struct CScore
{
	int score = 0;
	CScore(int s)
		: score(s) {}
};

struct CLifespan
{
	int remaining	= 0; // amount of lifespan remaining on the entity
	int total		= 0; // the total initial amount of lifespan
	CLifespan(int total)
		: remaining(total), total(total) {}
};