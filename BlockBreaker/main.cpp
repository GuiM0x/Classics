#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"

///////////////////////////////////////////////////
/////// CONST
const unsigned WINDOW_W = 1024;
const unsigned WINDOW_H = 576;
const float       PAD_W = 150.f;
const float       PAD_H = 25.f;
const float       PAD_Y = WINDOW_H - (PAD_H + 10.f);

///////////////////////////////////////////////////
/////// ENUM KEYS
enum keys{LEFT, RIGHT, KEY_MAX};
std::vector<bool> key(KEY_MAX, false);

///////////////////////////////////////////////////
/////// CLASS PADDLE (PROTO)
class Paddle : public sf::RectangleShape
{
public:
    explicit Paddle(const sf::Vector2f& size);

private:

};
/////// CLASS PADDLE (DEF)
Paddle::Paddle(const sf::Vector2f& size) :
    sf::RectangleShape{size}
{

}
///////////////////////////////////////////////////
/////// MAIN
int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 576), "Block Breaker", sf::Style::Close);

    /////// Paddle
    Paddle pad{sf::Vector2f(PAD_W, PAD_H)};
    pad.setPosition(0.f, PAD_Y);

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
                if (event.key.code == sf::Keyboard::Q) { key[LEFT]  = true; }
                if (event.key.code == sf::Keyboard::D) { key[RIGHT] = true; }

                if(event.key.code == sf::Keyboard::Escape)
                    window.close();
            }

            /////// KEY RELEASED
			if (event.type == sf::Event::KeyReleased)
			{
				//Directions
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
        window.draw(pad);
        window.display();
    }

    return 0;
}
