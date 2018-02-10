#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "include/Outils.h"
#include "../../Utilities/Matrix.hpp"

/// ///////////////////////////////////////////////
/// CONST
const std::size_t GRID_ROWS = 8;
const std::size_t GRID_COLS = 10;
const unsigned     WINDOW_W = 1024;
const unsigned     WINDOW_H = 576;
const float         BLOCK_W = 70.f;
const float         BLOCK_H = 25.f;
const float           PAD_W = 150.f;
const float           PAD_H = 20.f;
const float           PAD_X = (WINDOW_W / 2.f) - (PAD_W / 2.f);
const float           PAD_Y = WINDOW_H - (PAD_H + 10.f);
const float           SPEED = 100.f;
const float              PI = 3.141592f;

const sf::Vector2f origin_grid{(WINDOW_W - (GRID_COLS*BLOCK_W)) / 2.f, 0.f};

/// ///////////////////////////////////////////////
/// ENUMS
enum keys{LEFT, RIGHT, KEY_MAX};
std::vector<bool> key(KEY_MAX, false);

/// ///////////////////////////////////////////////
/// CLASS PADDLE
class Paddle : public sf::RectangleShape
{
public:
    Paddle() = delete;
    explicit Paddle(const sf::Vector2f& size) :
        sf::RectangleShape{size} {}

    void move(const sf::Time& dt, int dir);

private:

};
//////////////////////
void Paddle::move(const sf::Time& dt, int dir)
{
    switch(dir){
        case keys::LEFT  : sf::RectangleShape::move(-SPEED*dt.asSeconds(), 0.f); break;
        case keys::RIGHT : sf::RectangleShape::move(SPEED*dt.asSeconds(), 0.f); break;
        default :
            break;
    }
}
/// ///////////////////////////////////////////////
/// CLASS BLOCK
class Block : public sf::RectangleShape
{
public:
    Block();
    Block(const sf::Vector2f&);
    ~Block() {}

private:

};
//////////////////////
Block::Block() :
    sf::RectangleShape{}
{

}

Block::Block(const sf::Vector2f& size) :
    sf::RectangleShape{size}
{

}

/// ///////////////////////////////////////////////
/// CLASS BALL
class Ball : public sf::CircleShape
{
public:
    Ball() = delete;
    Ball(float radius=0, std::size_t pointCount=30);

    void move(const sf::Time&);
    void bounce(const Paddle&);
    void bounce(const Block&);
    sf::FloatRect getGlobalBounds() const;

private:

    friend std::ostream& operator<<(std::ostream& os, const Ball& b){
        os << "Ball Spec(s) :\n"
           << "rotation -> " << b.getRotation() << '\n'
           << "------------------------\n";

        return os;
    }
};
//////////////////////
Ball::Ball(float radius, std::size_t pointCount) :
    sf::CircleShape{radius, pointCount}
{
    setOrigin(radius, radius);
}

void Ball::move(const sf::Time& dt)
{
    const float angle_deg{getRotation()};
    const float angle_rad{(PI * angle_deg) / 180.f};

    sf::CircleShape::move(cos(angle_rad) * SPEED * dt.asSeconds(),
                          sin(angle_rad) * SPEED * dt.asSeconds());
}

void Ball::bounce(const Paddle& p)
{
    const float distFromMidPad{getPosition().x - (p.getPosition().x + (p.getGlobalBounds().width/2.f))};
    const float max_bounce_angle{45.f};
    const float degPerPixel{max_bounce_angle/(PAD_W/2.f)};

    // Left part of paddle
    if(distFromMidPad < 0){
        setRotation(270.f - ((distFromMidPad * -1.f) * degPerPixel));
    }
    // Right part of paddle
    else if(distFromMidPad > 0){
        setRotation(270.f + (distFromMidPad * degPerPixel));
    }
    else{
        setRotation(270.f);
    }
}

sf::FloatRect Ball::getGlobalBounds() const
{
    return sf::FloatRect{{getPosition().x - getRadius(), getPosition().y - getRadius()},
                         {sf::CircleShape::getGlobalBounds().width, sf::CircleShape::getGlobalBounds().height}};
}

void Ball::bounce(const Block& block)
{
    // Left
    if(getGlobalBounds().left <= block.getPosition().x &&
       getGlobalBounds().left + getGlobalBounds().width >= block.getPosition().x)
    {

    }

    // Right
    if(getGlobalBounds().left <= block.getPosition().x + block.getGlobalBounds().width &&
       getGlobalBounds().left + getGlobalBounds().width >= block.getPosition().x + block.getGlobalBounds().width)
    {

    }

    // Middle
    if(getGlobalBounds().left >= block.getPosition().x &&
       getGlobalBounds().left <= block.getPosition().x + block.getGlobalBounds().width)
    {
        // Top ?

        // Bottom ?
    }
}

/// ///////////////////////////////////////////////
/// OTHER FUNCTION(S)
// Create Block
Block createBlock(float, float);
Block createBlock(float width, float height)
{
    Block tmp{sf::Vector2f(width, height)};
    tmp.setFillColor(sf::Color(Outils::rollTheDice(0,1),
                               Outils::rollTheDice(25,255),
                               Outils::rollTheDice(0,1)));

    return tmp;
}
// Collide With Paddle
bool collideWithPaddle(const Ball&, const Paddle&);
bool collideWithPaddle(const Ball& b, const Paddle& p)
{
    if((b.getPosition().x + b.getRadius()) >= p.getPosition().x &&
       (b.getPosition().x - b.getRadius()) <= p.getPosition().x + PAD_W)
    {
        if((b.getPosition().y + b.getRadius()) >= p.getPosition().y &&
           (b.getPosition().y - b.getRadius()) <= p.getPosition().y)
        {
            return true;
        }

        return false;
    }

    return false;
}
// Collide With Block
bool collideWithBlock(const Ball&, const Block&);
bool collideWithBlock(const Ball& ball, const Block& block)
{
    return (ball.getGlobalBounds().intersects(block.getGlobalBounds()));
}
/// ///////////////////////////////////////////////
/// MAIN
int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 576), "Block Breaker", sf::Style::Close);

    /////// Paddle
    Paddle pad{sf::Vector2f(PAD_W, PAD_H)};
    pad.setPosition(PAD_X, PAD_Y);

    /////// Ball
    Ball myBall{12.f};
    myBall.setRotation(270.f);
    myBall.setPosition(WINDOW_W/2.f, PAD_Y - 24.f);

    /////// Blocks Grid
    Matrix<Block> gridBlocks(GRID_ROWS, GRID_COLS, Block{});
    for(std::size_t i=0; i<GRID_ROWS; ++i){
        for(std::size_t j=0; j<GRID_COLS; ++j){
            gridBlocks(i, j) = createBlock(BLOCK_W, BLOCK_H);
            gridBlocks(i, j).setPosition((j*BLOCK_W)+origin_grid.x, i*BLOCK_H);
        }
    }

    Block oneBlock{{70.f, 25.f}};
    oneBlock.setPosition(WINDOW_W/2.f, WINDOW_H/2.f);

    /// DEBUG
    sf::RectangleShape boxBall{{myBall.getGlobalBounds().width, myBall.getGlobalBounds().height}};
    boxBall.setFillColor(sf::Color::Transparent);
    boxBall.setOutlineThickness(1);
    boxBall.setOutlineColor(sf::Color::Red);
    /// END DEBUG

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

                /// DEBUG
                if (event.key.code == sf::Keyboard::Space) { std::cout << myBall << '\n'; }
			}

            /////// MOUSE PRESSED
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    myBall.rotate(36.f);
                }
                if(event.mouseButton.button == sf::Mouse::Right) {
                    myBall.rotate(-36.f);
                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        /////// UPDATE
        if(key[LEFT])  pad.move(dt, keys::LEFT);
        if(key[RIGHT]) pad.move(dt, keys::RIGHT);

        if(collideWithPaddle(myBall, pad)){
            myBall.bounce(pad);
        }

        if(collideWithBlock(myBall, oneBlock)){
            std::cout << "Collide with block !" << '\n';
        }

        myBall.move(dt);

        /// DEBUG
        boxBall.setPosition(myBall.getGlobalBounds().left, myBall.getGlobalBounds().top);
        /// END DEBUF

        /////// DRAW
        window.clear();
        window.draw(pad);
        window.draw(myBall);
        /*for(const auto& block : gridBlocks)
            window.draw(block);*/
        window.draw(oneBlock);
        window.draw(boxBall);
        window.display();
    }

    return 0;
}
