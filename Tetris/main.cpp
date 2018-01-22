#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <cassert>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"

///////////////////////////////
/////// CREATE PIECE
std::vector<sf::Sprite> createPiece(const std::vector<unsigned>& patron, const sf::Sprite& s, const sf::Sprite& empty_s)
{
    double sizeMatrice{sqrt(patron.size())};
    assert(round(sizeMatrice) == sizeMatrice && "Piece must be a squared matrice.");
    sizeMatrice = static_cast<std::size_t>(sizeMatrice);

    std::vector<sf::Sprite> tmp;

    for(std::size_t i=0; i<sizeMatrice; ++i){
        for(std::size_t j=0; j<sizeMatrice; ++j){

            // Provisoire
            if(patron[(i*sizeMatrice)+j] == 1){
                tmp.push_back(s);
            }
            else{
                //tmp.push_back(empty_s); /// DEBUG
                tmp.push_back(sf::Sprite());
            }

            tmp.back().setPosition(j*s.getGlobalBounds().width, i*s.getGlobalBounds().height);
        }
    }

    return tmp;
}
///////////////////////////////
/////// MOVE PIECE
void movePiece(std::vector<sf::Sprite>& v, int dx = 0, int dy = 0)
{
    for(auto&& x : v){
        x.move(20*dx, 20*dy);
    }
}
///////////////////////////////
/////// ROTATE PIECE (only clockwise for the moment)
void rotatePiece(std::vector<sf::Sprite>& piece)
{
    double sizeMatrice{sqrt(piece.size())};
    unsigned rows{static_cast<unsigned>(sizeMatrice)};
    unsigned cols{rows};

    std::vector<sf::Sprite> tmp;

    // Start Reading bottom-left
    std::size_t start{piece.size() - cols};

    for(std::size_t i=start; i<piece.size(); ++i){
        std::size_t index{i};
        for(unsigned j=0; j<rows; ++j){
            tmp.push_back(piece[index]); // Linear push to tmp
            index -= cols;
        }
    }

    for(unsigned i=0; i<rows; ++i){
        for(unsigned j=0; j<cols; ++j){
            tmp[(i*cols)+j].setPosition(piece[(i*cols)+j].getPosition());
        }
    }

    piece = tmp;
}

///////////////////////////////
int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 576), "Tetris");

    sf::Texture t;
    t.loadFromFile("assets/img/tiles.png");
    sf::Sprite s(t);

    /// DEBUG
    sf::Texture empty_t;
    empty_t.loadFromFile("assets/img/empty_tile.png");
    sf::Sprite empty_s(empty_t);

    // Pattern default : {0, 0, 0, 0, 0, 0, 0, 0, 0} (9);
    // Pattern default : {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} (16);
    const std::vector<std::vector<unsigned>> patrons{{0, 1, 0, 1, 1, 1, 0, 0, 0},  // T
                                                     {1, 0, 0, 1, 1, 1, 0, 0, 0},  // J
                                                     {0, 0, 1, 1, 1, 1, 0, 0, 0},  // L
                                                     {0, 1, 1, 1, 1, 0, 0, 0, 0},  // S
                                                     {1, 1, 0, 0, 1, 1, 0, 0, 0},  // Z
                                                     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // I
                                                     {1, 1, 1, 1},
                                                     {0, 1, 0, 1, 1, 1, 0, 1, 0},  // +
                                                     {1, 1, 1, 0, 1, 0, 1, 0, 0},  // >
                                                     {0, 1, 1, 0, 1, 0, 1, 1, 0},  // Big S
                                                     {1, 1, 0, 0, 1, 0, 0, 1, 1},  // Big Z
                                                     {0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0}}; // BOOMRANG

    // Create one piece
    std::vector<sf::Sprite> piece{createPiece(patrons[0], s, empty_s)};

    // Create PlayField
    sf::RectangleShape playField{sf::Vector2f(200.f, 400.f)};
    playField.setFillColor(sf::Color(230, 230, 230));
    const float playFieldLeft{playField.getGlobalBounds().left};
    const float playFieldRight{playField.getGlobalBounds().left + playField.getGlobalBounds().width};
    const float playFieldDown{playField.getGlobalBounds().top + playField.getGlobalBounds().height};

    /////// CLOCK/DT
    sf::Clock clock;
    sf::Time dt;
    float timer{0.f};   // Used for auto move down
    float delayMax{1.f}; // Used to control the auto move speed, less is the number, faster is the descent

    /////// GAME LOOP
    while (window.isOpen())
    {
        dt = clock.restart();
        timer += dt.asSeconds();

        /////// EVENTS
        sf::Event event;
        while (window.pollEvent(event))
        {
            /////// KEY PRESSED
            if(event.type == sf::Event::KeyPressed)
            {
                // PRESSED DOWN
				if (event.key.code == sf::Keyboard::S) {
                        // TO DO : Test empty sprite
                        auto find_collide = std::find_if(piece.begin(), piece.end(),
                                                         [playFieldDown](const sf::Sprite& s)
                                                            { return (s.getGlobalBounds().top + s.getGlobalBounds().height >= playFieldDown && s.getTexture() != nullptr); });
                        if(find_collide == piece.end())
                            movePiece(piece, 0, 1);
                }
                // PRESSED LEFT
				if (event.key.code == sf::Keyboard::Q) {
                        // TO DO : Test empty sprite
                        auto find_collide = std::find_if(piece.begin(), piece.end(),
                                                         [playFieldLeft](const sf::Sprite& s)
                                                            { return (s.getGlobalBounds().left <= playFieldLeft && s.getTexture() != nullptr); });
                        if(find_collide == piece.end())
                            movePiece(piece, -1, 0);
                }
                // PRESSED RIGHT
				if (event.key.code == sf::Keyboard::D) {
                        // TO DO : Test empty sprite
                        auto find_collide = std::find_if(piece.begin(), piece.end(),
                                                         [playFieldRight](const sf::Sprite& s)
                                                            { return (s.getGlobalBounds().left + s.getGlobalBounds().width >= playFieldRight && s.getTexture() != nullptr); });
                        if(find_collide == piece.end())
                            movePiece(piece, 1, 0);
				}
				// PRESSED ROTATE
				if (event.key.code == sf::Keyboard::R) {
                    rotatePiece(piece);
				}
                // PRESSED ESCAPE
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
        if(timer >= delayMax) {
            auto find_collide = std::find_if(piece.begin(), piece.end(),
                                             [playFieldDown](const sf::Sprite& s)
                                                 { return (s.getGlobalBounds().top + s.getGlobalBounds().height >= playFieldDown && s.getTexture() != nullptr); });
            if(find_collide == piece.end())
                movePiece(piece, 0, 1);

            timer = 0.f;
        }

        /////// DRAW
        window.clear();
        window.draw(playField);
        for(const auto& part : piece)
            window.draw(part);
        window.display();
    }

    return 0;
}
