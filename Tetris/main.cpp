#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"

///////////////////////////////
/////// MOVE PIECE
void movePiece(std::vector<sf::Sprite>& v, int dx = 0, int dy = 0)
{
    for(auto&& x : v){
        x.move(20*dx, 20*dy);
    }
}
///////////////////////////////
int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 576), "Tetris");

    sf::Texture t;
    t.loadFromFile("assets/img/tiles.png");

    sf::Sprite s(t);

    // Pieces represented in 2D Grid [7][4]
    const std::vector<std::vector<unsigned>> pieces{
        {0,2,4,6}, // I
        {2,4,6,7}, // L
        {3,5,6,7}, // J
        {4,5,6,7}, // O
        {3,4,5,6}, // Z
        {2,4,5,7}, // S
        {2,4,5,6}  // T
    };

    // One Piece
    std::size_t n{6};
    std::vector<sf::Sprite> piece;
    for(std::size_t i = 0; i < 4; ++i){
        piece.push_back(s);
        piece.back().setPosition((pieces[n][i]%2)*20, (pieces[n][i]/2)*20);
    }

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
				if (event.key.code == sf::Keyboard::S) { movePiece(piece, 0, 1);  }
				if (event.key.code == sf::Keyboard::Q) { movePiece(piece, -1, 0); }
				if (event.key.code == sf::Keyboard::D) { movePiece(piece, 1, 0);  }

				if(event.key.code == sf::Keyboard::Escape)
                    window.close();
            }

            /////// KEY RELEASED
			if (event.type == sf::Event::KeyReleased)
			{

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
        for(const auto& x : piece)
            window.draw(x);
        window.display();
    }

    return 0;
}
