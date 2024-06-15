#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include "Game.h"

#include <iostream>

int main()
{
	Game game("config.txt");
	game.run();
}
