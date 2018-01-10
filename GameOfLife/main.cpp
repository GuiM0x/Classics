#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"
#include "include/Grille.h"

///////////////////////////////
int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 576), "Sans Titre", sf::Style::Close);

    /////// GRILLE
    Grille grid(&window, 72, 128, 8, 8);
    grid.fillWithRectangle();
    grid.fillWithCell();

    /////// FPS TEXT
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text fpsText("", font, 16);
    fpsText.setPosition(10,10);
    fpsText.setFillColor(sf::Color::Green);

    /////// VARS
    sf::Clock clock;
    sf::Time dt;
    float fps;
    float elapsed{0};
    bool AUTOMATA{false};
    sf::Color backgroundColor(sf::Color(61,61,61));

    /////// GAME LOOP
    while (window.isOpen())
    {
        dt = clock.restart();
        elapsed += dt.asSeconds();
        fps = 1 / dt.asSeconds();

        /////// EVENTS
        sf::Event event;
        while (window.pollEvent(event))
        {
            /////// KEY PRESSED
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Space) {
                    (AUTOMATA) ?
                    AUTOMATA = false :
                        AUTOMATA = true;
                    (AUTOMATA) ?
                    std::cout << "AUTOMATA actived" << '\n' :
                        std::cout << "AUTOMATA Paused" << '\n';
                }
                if(event.key.code == sf::Keyboard::C) {
                    AUTOMATA = false;
                    std::cout << "STATES reset and AUTOMATA desactived" << '\n';
                    grid.resetLife();
                }
                if(event.key.code == sf::Keyboard::R) {
                    if(!AUTOMATA)
                        grid.genereRandCells();
                    else
                        std::cout << "AUTOMOATA must be desactived/paused to generate random grid" << '\n';
                }
            }

            /////// KEY RELEASED
			if (event.type == sf::Event::KeyReleased) {
				if(event.key.code == sf::Keyboard::Escape) {
                    window.close();
				}
			}

            /////// MOUSE PRESSED
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    grid.switchCellByClick();
                }
                if(event.mouseButton.button == sf::Mouse::Right) {
                    backgroundColor = sf::Color(Outils::rollTheDice(0,255),
                                                Outils::rollTheDice(0,255),
                                                Outils::rollTheDice(0,255));
                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        /////// UPDATE
        grid.update(AUTOMATA, dt);
        fpsText.setString(std::to_string(static_cast<unsigned>(fps)));

        /////// DRAW
        window.clear(backgroundColor);
        window.draw(grid);
        window.draw(fpsText);
        window.display();
    }

    return 0;
}
