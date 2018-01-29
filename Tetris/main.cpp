#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <ctime>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"

/// TO DO : Save scores to file.txt

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
                                                 {1, 1, 1, 1}}; // O
const sf::Vector2f originField{120.f, 40.f};
const unsigned     rowsGrid{20};
const unsigned     colsGrid{10};
const float        size_tile{20};
const float        delayMin{0.1f};
const float        delayMax{1.f};
const unsigned     SCREEN_X{500};
const unsigned     SCREEN_Y{480};

float              delay{delayMax};
unsigned           line_ctr{0};

//////////////////////////////////////////////////////////
/////// CREATE PIECE
std::vector<sf::Sprite> createPiece(const std::vector<unsigned>&, const sf::Sprite&);
std::vector<sf::Sprite> createPiece(const std::vector<unsigned>& patron, const sf::Sprite& s)
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
                tmp.push_back(sf::Sprite());
            }

            tmp.back().setPosition((j*s.getGlobalBounds().width)+(originField.x + (3*size_tile)),
                                   (i*s.getGlobalBounds().height)+originField.y);
        }
    }

    return tmp;
}
//////////////////////////////////////////////////////////
/////// MOVE PIECE
void movePiece(std::vector<sf::Sprite>&, int, int);
void movePiece(std::vector<sf::Sprite>& v, int dx = 0, int dy = 0)
{
    for(auto&& x : v){
        x.move(size_tile*dx, size_tile*dy);
    }
}
//////////////////////////////////////////////////////////
/////// CHECK EMPTY SPRITES OUTSIDE PLAYFIELD (used in rotation)
bool checkSpriteOut(const std::vector<sf::Sprite>&);
bool checkSpriteOut(const std::vector<sf::Sprite>& piece)
{
    for(const auto& part : piece){
        if(part.getPosition().x < originField.x)
            return true;
        if(part.getPosition().x >= (colsGrid*size_tile)+originField.x)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////
/////// CHECK EMPTY SPRITES INTERSECT CONCRETE SPRITES (used in event keyboard)
bool checkSpriteIntersect(const std::vector<sf::Sprite>&, const std::vector<sf::Sprite>&);
bool checkSpriteIntersect(const std::vector<sf::Sprite>& piece, const std::vector<sf::Sprite>& gridSprite)
{
    for(const auto& part : piece){
        for(const auto& sprite : gridSprite){
            if(sprite.getTexture() != nullptr){
                if(part.getPosition().x == sprite.getPosition().x &&
                   part.getPosition().y == sprite.getPosition().y)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//////////////////////////////////////////////////////////
/////// ROTATE PIECE (only clockwise for the moment)
void rotatePiece(std::vector<sf::Sprite>&);
void rotatePiece(std::vector<sf::Sprite>& piece)
{
    if(!checkSpriteOut(piece)){

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
}
//////////////////////////////////////////////////////////
/////// COLLIDE PLAYFIELD'S BORDERS
bool collidePlayField(const std::vector<sf::Sprite>&, float, unsigned);
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
/////// COLLIDE PIECE TO PIECE
bool collidePiece(const std::vector<sf::Sprite>&, std::vector<sf::Sprite>&, int);
bool collidePiece(const std::vector<sf::Sprite>& gridSprites, std::vector<sf::Sprite>& piece, int dir = dir::DOWN)
{
    for(auto&& part : piece){
        for(auto&& concretePart : gridSprites){
            if(part.getTexture() != nullptr && concretePart.getTexture() != nullptr){
                // BOTTOM-TOP
                if(dir == dir::DOWN){
                    if(part.getPosition().x == concretePart.getPosition().x){
                        if(part.getPosition().y+part.getGlobalBounds().height == concretePart.getPosition().y){
                            return true;
                        }
                    }
                }
                // LEFT
                if(dir == dir::LEFT){
                    if(part.getPosition().y == concretePart.getPosition().y){
                        if(part.getPosition().x == concretePart.getPosition().x + concretePart.getGlobalBounds().width){
                            return true;
                        }
                    }
                }
                // RIGHT
                if(dir == dir::RIGHT){
                    if(part.getPosition().y == concretePart.getPosition().y){
                        if(part.getPosition().x + part.getGlobalBounds().width == concretePart.getPosition().x){
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}
//////////////////////////////////////////////////////////
/////// RANDOM ID
std::size_t randomID(int, int);
std::size_t randomID(int minVal, int maxVal)
{
    assert((minVal>=0 && maxVal>=0) && "Index can't be negative.");
    assert((minVal<=static_cast<int>(patrons.size()) &&
            maxVal<=static_cast<int>(patrons.size())) &&
           "Index max out of range");

    return static_cast<std::size_t>(Outils::rollTheDice(minVal, maxVal));
}
//////////////////////////////////////////////////////////
//// MOVE ACTIVE PIECE INTO GRID
void movePieceToGrid(std::vector<sf::Sprite>& piece, std::vector<bool>&, std::vector<sf::Sprite>&);
void movePieceToGrid(std::vector<sf::Sprite>& piece, std::vector<bool>& grid, std::vector<sf::Sprite>& gridSprite)
{
    float x{0.f}, y{0.f};
    std::size_t i{0}, j{0};
    std::size_t index{0};

    for(auto&& part : piece){

        const float sizePart{part.getGlobalBounds().width};
        x = part.getPosition().x - originField.x;
        y = part.getPosition().y - originField.y;
        i = static_cast<std::size_t>(y/sizePart);
        j = static_cast<std::size_t>(x/sizePart);
        index = (i*colsGrid) + j;

        if(part.getTexture() != nullptr){
            grid[index] = true;
        }
        else{
            grid[index] = false;
        }

        gridSprite[index] = part;
    }

    piece.clear();
}
//////////////////////////////////////////////////////////
/////// LAUNCH NEXT PIECE
void launchNextPiece(std::vector<sf::Sprite>& activePiece, std::vector<sf::Sprite>&, const std::vector<sf::Sprite>);
void launchNextPiece(std::vector<sf::Sprite>& activePiece, std::vector<sf::Sprite>& nextPiece, const std::vector<sf::Sprite> tileSet)
{
    activePiece = std::move(nextPiece);
    nextPiece.clear();
    std::size_t randIndex{randomID(0, 6)};
    nextPiece = createPiece(patrons[randIndex], tileSet[randIndex]);
}
//////////////////////////////////////////////////////////
/////// CHECK LINES
std::vector<std::size_t> checkFullLines(const std::vector<bool>&);
std::vector<std::size_t> checkFullLines(const std::vector<bool>& grid)
{
    std::vector<std::size_t> tmp;

    for(std::size_t i=0; i<(rowsGrid*colsGrid); i+=colsGrid){
        auto it_begin = grid.begin() + i;
        auto it_end   = it_begin + colsGrid;
        auto it_find  = std::find(it_begin, it_end, false);
        if(it_find == it_end){
            tmp.push_back(i);
        }
    }

    return tmp;
}
//////////////////////////////////////////////////////////
/////// MOVE DOWN LINES : called at the end of eraseLines()
void moveDownLines(std::vector<bool>& grid, std::vector<sf::Sprite>&, std::size_t);
void moveDownLines(std::vector<bool>& grid, std::vector<sf::Sprite>& gridSprite, std::size_t lineErased)
{
    std::size_t startCopy{0};
    std::size_t endCopy{lineErased};

    for(std::size_t i=0; i<(rowsGrid*colsGrid); i+=colsGrid){
        auto it_begin = grid.begin() + i;
        auto it_end   = it_begin + colsGrid;
        auto it_find  = std::find(it_begin, it_end, true);
        if(it_find != it_end){
            startCopy = i;
            break;
        }
    }

    std::vector<bool> tmp_bool;
    std::vector<sf::Sprite> tmp_sprites;

    for(unsigned i=startCopy; i<endCopy; ++i){
        tmp_bool.push_back(grid[i]);
        tmp_sprites.push_back(gridSprite[i]);
    }
    for(unsigned i=startCopy; i<endCopy; ++i){
        grid[i] = false;
        gridSprite[i] = sf::Sprite();
    }
    for(unsigned i=0; i<tmp_bool.size(); ++i){
        grid[(startCopy+colsGrid)+i] = tmp_bool[i];
        gridSprite[(startCopy+colsGrid)+i] = tmp_sprites[i];
        gridSprite[(startCopy+colsGrid)+i].move(0, size_tile);
    }
}
//////////////////////////////////////////////////////////
/////// ERASE LINES
void eraseLines(std::vector<bool>&, std::vector<sf::Sprite>&);
void eraseLines(std::vector<bool>& grid, std::vector<sf::Sprite>& gridSprite)
{
    std::vector<std::size_t> linesToErase = checkFullLines(grid);

    for(auto&& line : linesToErase){
        for(std::size_t i=line; i<line+colsGrid; ++i){
            grid[i] = false;
        }

        ++line_ctr;

        if(line_ctr%5 == 0){

            if(delay>0.5f)
                delay -= 0.075f;
            else
                delay -=0.05;

            if(delay<delayMin)
                delay = delayMin;
        }

        moveDownLines(grid, gridSprite, line);
    }
}
//////////////////////////////////////////////////////////
/////// UPDATE NEXT PIECE TO SHOW
void updateNextPieceShow(const std::vector<sf::Sprite>&, std::vector<sf::Sprite>&);
void updateNextPieceShow(const std::vector<sf::Sprite>& nextPiece, std::vector<sf::Sprite>& nextPiecetoShow)
{
    nextPiecetoShow.clear();
    nextPiecetoShow = nextPiece;

    if(nextPiecetoShow.size() == 4){
        for(auto&& part : nextPiecetoShow){
            part.move(360.f-(originField.x+(3*size_tile)), 250.f-originField.y);
        }
    } else if(nextPiecetoShow.size() == 9){
        for(auto&& part : nextPiecetoShow){
            part.move(350.f-(originField.x+(3*size_tile)), 250.f-originField.y);
        }
    } else if(nextPiecetoShow.size() == 16){
        for(auto&& part : nextPiecetoShow){
            part.move(340.f-(originField.x+(3*size_tile)), 240.f-originField.y);
        }
    }
}
//////////////////////////////////////////////////////////
/////// SET TEXT LINES
void updateTextLines(sf::Text&, unsigned);
void updateTextLines(sf::Text& text, unsigned nb_lines)
{
    std::string tmp{std::to_string(nb_lines)};
    text.setString(tmp);
    text.setOrigin(text.getGlobalBounds().width / 2.f, text.getCharacterSize()/2.f);
}
//////////////////////////////////////////////////////////
/////// CHECK GAME OVER
bool checkGameOver(const std::vector<sf::Sprite>&, float);
bool checkGameOver(const std::vector<sf::Sprite>& gridSprite, float playFieldTop)
{
    unsigned ctr{0};

    for(std::size_t i=0; i<colsGrid; ++i){
        if(gridSprite[i].getPosition().y == playFieldTop)
            ++ctr;
        if(ctr>1)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////
/////// RESET GRIDS
void resetGrids(std::vector<bool>&, std::vector<sf::Sprite>&);
void resetGrids(std::vector<bool>& grid, std::vector<sf::Sprite>& gridSprite)
{
    for(auto&& b : grid)
        b = false;

    for(unsigned i=0; i<rowsGrid; ++i){
        for(unsigned j=0; j<colsGrid; ++j){
            gridSprite[(i*colsGrid)+j] = sf::Sprite();
            gridSprite.back().setPosition((j*size_tile)+originField.x, (i*size_tile)+originField.y);
        }
    }
}
//////////////////////////////////////////////////////////
/////// SAVE SCORE
void saveScore(unsigned, const std::string&);
void saveScore(unsigned score, const std::string& fileName)
{
    std::ofstream osf(fileName, std::ios_base::app);
    std::time_t result = std::time(nullptr);
    osf << score << ' ' << std::asctime(std::localtime(&result));
    osf.close();
}
/// //////////////////////////////////////////////////////
/// DEBUG
void printGrid(const std::vector<bool>& grid)
{
    for(std::size_t i=0; i<rowsGrid; ++i){
        for(std::size_t j=0; j<colsGrid; ++j){
            std::cout << ((grid[(i*colsGrid)+j] == true) ? "1 " : "0 ");
        }
        std::cout << '\n';
    }
    std::cout << "----\n";
}
//////////////////////////////////////////////////////////
/////// MAIN
int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_X, SCREEN_Y), "Tetrox", sf::Style::Close);

    /////// Texture & Sprite
    sf::Texture tiles;
    tiles.loadFromFile("assets/img/tiles_set.png");
    std::vector<sf::Sprite> tileSet;
    int sizeRect{static_cast<int>(size_tile)};
    int nb_tiles{static_cast<int>(tiles.getSize().x/size_tile)};
    for(int i=0; i<nb_tiles; ++i){
        tileSet.push_back(sf::Sprite{tiles, sf::IntRect{i*sizeRect, 0, sizeRect, sizeRect}});
    }

    /////// Background
    sf::Texture bg;
    bg.loadFromFile("assets/img/canva.png");
    sf::Sprite canva(bg);

    /////// Create piece
    std::size_t randIndex{randomID(0, 6)};
    std::vector<sf::Sprite> piece{createPiece(patrons[randIndex], tileSet[randIndex])};

    /////// Create Next Piece
    randIndex = randomID(0, 6);
    std::vector<sf::Sprite> nextPiece{createPiece(patrons[randIndex], tileSet[randIndex])};

    /////// Copy Next Piece (just for showing)
    std::vector<sf::Sprite> nextPiecetoShow(nextPiece.begin(), nextPiece.end());

    /////// Create PlayField
    sf::RectangleShape playField{sf::Vector2f(200.f, 400.f)};
    playField.setPosition(originField.x, originField.y);
    playField.setFillColor(sf::Color(230, 230, 230));
    const float playFieldTop{playField.getGlobalBounds().top};
    const float playFieldBottom{playField.getGlobalBounds().top + playField.getGlobalBounds().height};
    const float playFieldLeft{playField.getGlobalBounds().left};
    const float playFieldRight{playField.getGlobalBounds().left + playField.getGlobalBounds().width};
    std::vector<bool> grid(200, false);
    std::vector<sf::Sprite> gridSprites(200, sf::Sprite());

    /////// Create Text View Lines
    sf::Font digiFont;
    digiFont.loadFromFile("assets/fonts/DS-DIGI.ttf");
    sf::Text textLines("0", digiFont, 26);
    // Offset height between char size and global height
    const float offsetH{textLines.getCharacterSize() - textLines.getGlobalBounds().height};
    // Set text's top left on window's top left
    textLines.setPosition(0.f, -offsetH);
    textLines.setOrigin(textLines.getGlobalBounds().width / 2.f, textLines.getCharacterSize()/2.f);
    textLines.move(379.f, 384.f);

    /////// Game Over Screen Components
    sf::Texture go_texture;
    go_texture.loadFromFile("assets/img/GO_screen.png");
    sf::Sprite go_sprite(go_texture);

    /////// CLOCK/DT
    sf::Clock clock;
    sf::Time dt;
    float timer{0.f};   // Used for auto move down

    /////// STATES
    bool justLaunched{false};
    bool gameOver{false};

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
                    if(!collidePlayField(piece, playFieldBottom, dir::DOWN) &&
                       !collidePiece(gridSprites, piece, dir::DOWN))
                    {
                        movePiece(piece, 0, 1);
                    }
                }
                // PRESSED LEFT
                if (event.key.code == sf::Keyboard::Q) {
                    if(!collidePlayField(piece, playFieldLeft, dir::LEFT) &&
                       !collidePiece(gridSprites, piece, dir::LEFT))
                    {
                        movePiece(piece, -1, 0);
                    }
                }
                // PRESSED RIGHT
                if (event.key.code == sf::Keyboard::D) {
                    if(!collidePlayField(piece, playFieldRight, dir::RIGHT) &&
                       !collidePiece(gridSprites, piece, dir::RIGHT))
                    {
                        movePiece(piece, 1, 0);
                    }
                }
				// PRESSED ROTATE (right)
				if (event.key.code == sf::Keyboard::R) {
                    if(!checkSpriteIntersect(piece, gridSprites))
                    {
                        rotatePiece(piece);
                    }
				}

				// GAME OVER SCREEN KEYS
				if(gameOver){
                    if(event.key.code == sf::Keyboard::Return){
                        gameOver = false;
                    }
				}

                // PRESSED ESCAPE
                if(event.key.code == sf::Keyboard::Escape)
                    window.close();

                /// DEBUG PRINT (SHORTCUT)
                if(event.key.code == sf::Keyboard::Space){
                    printGrid(grid);
                    std::cout << "Total line(s) : " << line_ctr << '\n';
                    std::cout << "Descent Delay : " << delay << '\n';
                    std::cout << "------------------------------\n";
                }
            }

            /////// KEY RELEASED
            if (event.type == sf::Event::KeyReleased){
                // Nothing for the moment...
            }

            /////// MOUSE PRESSED
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    // Nothing for the moment...
                }
                if(event.mouseButton.button == sf::Mouse::Right) {
                    // Nothing for the moment...
                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        /////// UPDATE
        if(!gameOver){

            if(timer >= delay) {
                if(collidePiece(gridSprites, piece, dir::DOWN) ||
                   collidePlayField(piece, playFieldBottom, dir::DOWN))
                {
                    movePieceToGrid(piece, grid, gridSprites);
                    launchNextPiece(piece, nextPiece, tileSet);
                    justLaunched = true;
                }

                if(!collidePlayField(piece, playFieldBottom, dir::DOWN) && !justLaunched)
                {
                    movePiece(piece, 0, 1);
                }

                justLaunched = false;
                timer = 0.f;
            }

            if(checkGameOver(gridSprites, playFieldTop)){
                saveScore(line_ctr, "datas/scores");
                gameOver = true;
            }

        } else {
            // Reset all
            resetGrids(grid, gridSprites);
            line_ctr        = 0;
            delay           = delayMax;
            randIndex       = randomID(0, 6);
            piece           = createPiece(patrons[randIndex], tileSet[randIndex]);
            for(auto&& part : piece) part.move(0.f, -size_tile); // Avoid offset after GO screen
            randIndex       = randomID(0, 6);
            nextPiece       = createPiece(patrons[randIndex], tileSet[randIndex]);
            nextPiecetoShow = nextPiece;
        }

        // if(!gameOver) ? (à voir selon la présentation de l'écran de Game Over)
        if(!gameOver){
            eraseLines(grid, gridSprites);
            updateTextLines(textLines, line_ctr);
            updateNextPieceShow(nextPiece, nextPiecetoShow);
        }

        /////// DRAW
        window.clear();

        if(!gameOver){
            window.draw(canva);
            window.draw(textLines);
            for(const auto& part : piece)
                window.draw(part);
            for(const auto& s : gridSprites)
                window.draw(s);
            for(const auto& part : nextPiecetoShow)
                window.draw(part);
        }

        if(gameOver){
            // DRAW Screen GAME OVER
            window.draw(go_sprite);
        }

        window.display();
    }

    return 0;
}
