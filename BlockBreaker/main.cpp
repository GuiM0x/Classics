#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"

///////////////////////////////
/////// RESET KEY STATE
void reset_key_state(std::vector<bool>& v){
    for(auto&& x : v)
        x = false;
}

enum keys{UP, DOWN, LEFT, RIGHT, KEY_MAX};
std::vector<bool> key(KEY_MAX, false);
///////////////////////////////
int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 576), "Block Breaker");

    /////// CLOCK/DT
    sf::Clock clock;
    sf::Time dt;

    /////// GAME LOOP
    while (window.isOpen())
    {
        dt = clock.restart();

        /////// EVENTS
        sf::Event event;
        while (window.pollEvent(event))
        {
            /////// KEY PRESSED
            if(event.type == sf::Event::KeyPressed)
            {
                //Directions
				if (event.key.code == sf::Keyboard::Z) { key[UP]    = true; }
				if (event.key.code == sf::Keyboard::S) { key[DOWN]  = true; }
				if (event.key.code == sf::Keyboard::Q) { key[LEFT]  = true; }
				if (event.key.code == sf::Keyboard::D) { key[RIGHT] = true; }

				if(event.key.code == sf::Keyboard::Escape)
                    window.close();
            }

            /////// KEY RELEASED
			if (event.type == sf::Event::KeyReleased)
			{
				//Directions
				if (event.key.code == sf::Keyboard::Z) { key[UP]    = false; }
				if (event.key.code == sf::Keyboard::S) { key[DOWN]  = false; }
				if (event.key.code == sf::Keyboard::Q) { key[LEFT]  = false; }
				if (event.key.code == sf::Keyboard::D) { key[RIGHT] = false; }

			}

            /////// MOUSE PRESSED
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {

                }
                if(event.mouseButton.button == sf::Mouse::Right) {

                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        /////// UPDATE


        /////// DRAW
        window.clear();

        window.display();
    }

    return 0;
}
