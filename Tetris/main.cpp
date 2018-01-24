#include <iostream>
#include <cmath>
#include <cassert>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"

//////////////////////////////////////////////////////////
/////// ENUM DIRECTION
enum dir{
    DOWN,
    LEFT,
    RIGHT,
    DIR_MAX
};
//////////////////////////////////////////////////////////
/////// PATRONS OF PIECE
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
                                                 {0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0}}; // BOOMRANG
//////////////////////////////////////////////////////////
/////// CREATE PIECE
std::vector<sf::Sprite> createPiece(const std::vector<unsigned>& patron, const sf::Sprite& s, const sf::Sprite& empty_s)
{
    double sizeMatrice{sqrt(patron.size())};
    assert(round(sizeMatrice) == sizeMatrice && "Piece must be a squared matrice.");
    sizeMatrice = static_cast<std::size_t>(sizeMatrice);

    std::vector<sf::Sprite> tmp;

    for(std::size_t i=0; i<sizeMatrice; ++i){
        for(std::size_t j=0; j<sizeMatrice; ++j){
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
//////////////////////////////////////////////////////////
/////// MOVE PIECE
void movePiece(std::vector<sf::Sprite>& v, int dx = 0, int dy = 0)
{
    for(auto&& x : v){
        x.move(20*dx, 20*dy);
    }
}
//////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////
/////// COLLIDE PLAYFIELD'S BORDERS
bool collidePlayField(const std::vector<sf::Sprite>& piece, float playFieldBorder, unsigned dir = dir::DOWN)
{
    bool isCollide{false};

    for(const auto& s : piece){
        if(s.getTexture() != nullptr){
            if(dir == dir::DOWN){
                isCollide = (s.getGlobalBounds().top + s.getGlobalBounds().height >= playFieldBorder);
            }
            if(dir == dir::LEFT){
                isCollide = (s.getGlobalBounds().left <= playFieldBorder);
            }
            if(dir == dir::RIGHT){
                isCollide = (s.getGlobalBounds().left + s.getGlobalBounds().width >= playFieldBorder);
            }
            if(isCollide)
                break;
        }
    }

    return isCollide;
}
//////////////////////////////////////////////////////////
/////// COLLIDE PIECE TO PIECE (Y)
bool collidePieceY(const std::vector<std::vector<sf::Sprite>>& playFieldPieces, std::vector<sf::Sprite>& piece)
{
    for(const auto& concretePiece : playFieldPieces){
        for(const auto& concretePart : concretePiece){
            if(concretePart.getTexture() != nullptr){
                for(auto&& part : piece){
                    if(part.getTexture() != nullptr){
                        if(part.getPosition().x == concretePart.getPosition().x){
                            if(part.getPosition().y+part.getGlobalBounds().height == concretePart.getPosition().y){
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}
//////////////////////////////////////////////////////////
/////// COLLIDE PIECE TO PIECE (X LEFT)
bool collidePieceLeft(const std::vector<std::vector<sf::Sprite>>& playFieldPieces, std::vector<sf::Sprite>& piece)
{
    for(const auto& concretePiece : playFieldPieces){
        for(const auto& concretePart : concretePiece){
            if(concretePart.getTexture() != nullptr){
                for(auto&& part : piece){
                    if(part.getTexture() != nullptr){
                        if(part.getPosition().y == concretePart.getPosition().y){
                            if(part.getPosition().x == concretePart.getPosition().x + concretePart.getGlobalBounds().width){
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}
//////////////////////////////////////////////////////////
/////// COLLIDE PIECE TO PIECE (X RIGHT)
bool collidePieceRight(const std::vector<std::vector<sf::Sprite>>& playFieldPieces, std::vector<sf::Sprite>& piece)
{
    for(const auto& concretePiece : playFieldPieces){
        for(const auto& concretePart : concretePiece){
            if(concretePart.getTexture() != nullptr){
                for(auto&& part : piece){
                    if(part.getTexture() != nullptr){
                        if(part.getPosition().y == concretePart.getPosition().y){
                            if(part.getPosition().x + part.getGlobalBounds().width == concretePart.getPosition().x){
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}
//////////////////////////////////////////////////////////
/////// UPDATE PLAYFIELD'S GRID
void updateGrid(std::vector<bool>& grid, const std::vector<std::vector<sf::Sprite>>& playFieldPieces)
{
    float x{0.f}, y{0.f};
    std::size_t i{0}, j{0};
    std::size_t index{0};
    const unsigned cols{10};


    for(auto&& i : grid){
        i = false;
    }

    std::cout << playFieldPieces.size() << '\n'; /// DEBUG

    for(const auto& piece : playFieldPieces){
        for(const auto& part : piece){
            if(part.getTexture() != nullptr){
                const float sizePart{part.getGlobalBounds().width};
                x = part.getPosition().x;
                y = part.getPosition().y;
                i = static_cast<std::size_t>(y/sizePart);
                j = static_cast<std::size_t>(x/sizePart);
                index = (i*cols) + j;
                grid[index] = true;
            }
        }
    }
}
/// DEBUG
void printGrid(const std::vector<bool>& grid)
{
    std::cout << "----\n";
    for(std::size_t i=0; i<20; ++i){
        for(std::size_t j=0; j<10; ++j){
            std::cout << ((grid[(i*10)+j] == true) ? "1 " : "0 ");
        }
        std::cout << '\n';
    }
    std::cout << "----\n";
}
//////////////////////////////////////////////////////////
/////// RANDOM ID
std::size_t randomID(int minVal, int maxVal)
{
    assert((minVal>=0 && maxVal>=0) && "Index can't be negative.");
    assert((minVal<=static_cast<int>(patrons.size()) &&
            maxVal<=static_cast<int>(patrons.size())) &&
           "Index max out of range");

    return static_cast<std::size_t>(Outils::rollTheDice(minVal, maxVal));
}
//////////////////////////////////////////////////////////
/////// MOVE ACTIVE PIECE INTO PLAYFIELD PIECES
void moveToPlayfieldPieces(std::vector<sf::Sprite>& activePiece, std::vector<std::vector<sf::Sprite>>& playFieldPieces)
{
    playFieldPieces.push_back(std::move(activePiece));
    activePiece.clear();
}
//////////////////////////////////////////////////////////
/////// LAUNCH NEXT PIECE
void launchNextPiece(std::vector<sf::Sprite>& activePiece, std::vector<sf::Sprite>& nextPiece)
{
    activePiece = std::move(nextPiece);
    nextPiece.clear();
}
//////////////////////////////////////////////////////////
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

    /////// Create piece
    std::size_t index{randomID(0, patrons.size()-1)};
    std::vector<sf::Sprite> piece{createPiece(patrons[index], s, empty_s)};

    /////// Create Next Piece
    index = randomID(0, patrons.size()-1);
    std::vector<sf::Sprite> nextPiece{createPiece(patrons[index], s, empty_s)};

    /////// Vector that contains concrete pieces on PlayField
    std::vector<std::vector<sf::Sprite>> playFieldPieces;

    /////// Create PlayField
    sf::RectangleShape playField{sf::Vector2f(200.f, 400.f)};
    playField.setFillColor(sf::Color(230, 230, 230));
    //const float playFieldTop{playField.getGlobalBounds().top};
    const float playFieldBottom{playField.getGlobalBounds().top + playField.getGlobalBounds().height};
    const float playFieldLeft{playField.getGlobalBounds().left};
    const float playFieldRight{playField.getGlobalBounds().left + playField.getGlobalBounds().width};
    std::vector<bool> gridPlayField(200, false);

    /////// CLOCK/DT
    sf::Clock clock;
    sf::Time dt;
    float timer{0.f};   // Used for auto move down
    float delayMax{1.f}; // Used to control the auto move speed, less is the number, faster is the descent

    //////////////////////////////////////////////////////////
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
                    if(!collidePlayField(piece, playFieldBottom, dir::DOWN))
                        movePiece(piece, 0, 1);
                }
                // PRESSED LEFT
                if (event.key.code == sf::Keyboard::Q) {
                    if(!collidePlayField(piece, playFieldLeft, dir::LEFT) && !collidePieceLeft(playFieldPieces, piece))
                        movePiece(piece, -1, 0);
                }
                // PRESSED RIGHT
                if (event.key.code == sf::Keyboard::D) {
                    if(!collidePlayField(piece, playFieldRight, dir::RIGHT) && !collidePieceRight(playFieldPieces, piece))
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

            if(!collidePlayField(piece, playFieldBottom, dir::DOWN)){
                movePiece(piece, 0, 1);
            } else {
                std::cout << "Collide !" << '\n'; /// DEBUG
                moveToPlayfieldPieces(piece, playFieldPieces);
                launchNextPiece(piece, nextPiece);
                index     = randomID(0, patrons.size()-1);
                nextPiece = createPiece(patrons[index], s, empty_s);
            }

            updateGrid(gridPlayField, playFieldPieces);
            printGrid(gridPlayField);

            timer = 0.f;
        }

        // Ici la collision doit être en dehors de l'update basée sur le timer.
        // Cela évite la non détection de collision si une touche reste enfoncée.
        // Car l'update d'une touche est plus rapide, donc la piece passe au travers des autres.
        if(collidePieceY(playFieldPieces, piece)){
            std::cout << "Collide !" << '\n'; /// DEBUG
            moveToPlayfieldPieces(piece, playFieldPieces);
            launchNextPiece(piece, nextPiece);
            index     = randomID(0, patrons.size()-1);
            nextPiece = createPiece(patrons[index], s, empty_s);
        }

        /////// DRAW
        window.clear();
        window.draw(playField);
        for(const auto& part : piece)
            window.draw(part);
        for(const auto& piece : playFieldPieces){
            for(const auto& part : piece){
                window.draw(part);
            }
        }
        window.display();
    }

    return 0;
}
