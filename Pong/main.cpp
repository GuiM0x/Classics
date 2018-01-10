#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "include/Outils.h"

///////////////////////////////
/////// RESET KEY STATE
void reset_key_state(std::vector<bool>& v){
    for(auto&& x : v)
        x = false;
}

enum keys{P1_UP, P1_DOWN, P2_UP, P2_DOWN, KEY_MAX};
std::vector<bool> key(KEY_MAX, false);

///////////////////////////////
/////// CLASS BARRE
enum dir{
    UP   = -1,
    DOWN = 1
};

class Barre : public sf::RectangleShape
{
public:
    Barre(float width, float height) :
        RectangleShape(sf::Vector2f(width, height))
    {

    }

    float top() const { return getGlobalBounds().top; }
    float bottom() const { return (getGlobalBounds().top + getGlobalBounds().height); }
    float left() const { return getGlobalBounds().left; }
    float right() const { return (getGlobalBounds().left + getGlobalBounds().width); }

    void update(const sf::Time& dt, int direction) {
        if(direction < 0)
            setPosition(getPosition().x, (getPosition().y - m_speed * dt.asSeconds()));
        else
            setPosition(getPosition().x, (getPosition().y + m_speed * dt.asSeconds()));
    }

private:
    float m_speed = 250.f;
};
///////////////////////////////
/////// CLASS BALL
class Ball : public sf::CircleShape
{
public:
    explicit Ball(float radius = 0, std::size_t pointCount = 30) :
        CircleShape(radius, pointCount),
        m_box(sf::RectangleShape()),
        m_texture(sf::Texture())
    {
        m_texture.loadFromFile("assets/arrow_ball.png");
        m_texture.setSmooth(true);
        setTexture(&m_texture);
        setOrigin(radius, radius);
        m_box.setSize(sf::Vector2f(getGlobalBounds().width, getGlobalBounds().height));
        m_box.setFillColor(sf::Color::Transparent);
        m_box.setOutlineThickness(1);
        m_box.setOutlineColor(sf::Color::Red);
        setRotation(Outils::rollTheDice(1, 359));
    }

    float top() const { return getPosition().y; }
    float bottom() const { return (getPosition().y + getGlobalBounds().height); }
    float left() const { return getPosition().x; }
    float right() const { return (getGlobalBounds().left + getGlobalBounds().width); }

    void setLimit(float left, float right) {
        m_limitLeft = left;
        m_limitRight = right;
    }

    void collideWindow(sf::RenderTarget *window)
    {
        // Top
        if(top() <= getRadius()) {
            setPosition(left(), getRadius());
            bounceH();
        }

        // Bottom
        if(top() >= (window->getSize().y - getRadius())) {
            setPosition(left(), window->getSize().y - getRadius());
            bounceH();
        }

        //Left - Right
        if(left() <= getRadius() || left() >= window->getSize().x - getRadius()) {
            bounceV(); // Provisoire
        }
    }

    void collidePaddle(const Barre& p1, const Barre& p2)
    {
        sf::FloatRect player1(p1.getPosition(), p1.getSize());
        sf::FloatRect player2(p2.getPosition(), p2.getSize());

        // If Ball is inside player 1
        if(player1.intersects(m_box.getGlobalBounds())) {

            /// PARTIE TRICKY :
            /// Danc le cas ou un coin de balle et en intersection avec un coin de paddle,
            /// pour savoir vers quelle direction la balle doit rebondir,
            /// on récupère l'angle vers lequel elle pointe,
            /// ce qui permet de savoir si elle monte ou elle descend.
            /// PARTIE OBVIOUS :
            /// Dans le cas où la balle viens d'en bas,
            /// il lui est théoriquement impossible de toucher le haut du paddle,
            /// ce qui signifie qu'elle devra rebondir sur la zone verticale
            /// et bien sûr sinon elle rebondira sur la zone horizontale du paddle.
            /// PARTIE LAST CASE :
            /// Dernier cas de figure, si la balle n'est ni sur le coin du haut,
            /// ni sur le coin du bas alors bien sûr elle rebondit verticalement.

            // TO DO : Adjust position before bounce to avoid somes collision's bugs

            if(top() <= p1.top() && bottom() >= p1.top()) {
                // if come from down
                if(getRotation() > 180 && getRotation() < 270)
                    bounceV();
                else
                    bounceH();
            }
            else if(top() <= p1.bottom() && bottom() >= p1.bottom()) {
                // if come from up
                if(getRotation() > 90 && getRotation() < 180)
                    bounceV();
                else
                    bounceH();
            }
            else {
                bounceV();
            }
        }

        // If Ball is inside player 1
        if(player2.intersects(m_box.getGlobalBounds())) {

            if(top() <= p2.top() && bottom() >= p2.top()) {
                // if ball comes from down
                if(getRotation() > 0 && getRotation() < 90)
                    bounceV();
                else
                    bounceH();
            }
            else if(top() <= p2.bottom() && bottom() >= p2.bottom()) {
                // if ball comes from up
                if(getRotation() > 270 && getRotation() < 0)
                    bounceV();
                else
                    bounceH();
            }
            else {
                bounceV();
            }
        }
    }

    void update(const sf::Time& dt)
    {
        float radA = getRotation() * ((2.f * PI) / 360.f);
        float x    = left() + (cos(radA) * m_speed * dt.asSeconds());
        float y    = top() + (sin(radA) * m_speed * dt.asSeconds());
        setPosition(x, y);
        m_box.setPosition(x - getRadius(), y - getRadius());
    }

    sf::RectangleShape m_box;
private:
    sf::Texture  m_texture;
    const float  PI           = 3.141592f;
    const float  m_speed      = 700.f;
    float        m_limitLeft  = 0.f;
    float        m_limitRight = 0.f;

    void bounceH() {
        setRotation(360 - getRotation());
    }

    void bounceV() {
        setRotation(90 - (getRotation() - 90));
    }
};

std::ostream& operator<<(std::ostream& os, const Ball& b)
{
    os << "Rotation: " << b.getRotation() << " deg(s)";
    return os;
}
///////////////////////////////
/////// FUNC

///////////////////////////////
/////// CONST
const unsigned WINDOW_WIDTH  = 1024;
const unsigned WINDOW_HEIGHT = 576;
const float    BARRE_WIDTH   = 25;
const float    BARRE_HEIGHT  = 150;
const float    P1_X          = 10;
const float    P2_X          = WINDOW_WIDTH - BARRE_WIDTH - 10;
const float    P_Y           = (WINDOW_HEIGHT / 2) - (BARRE_HEIGHT / 2);
const float    BALL_RADIUS   = 16;
const float    BALL_X        = (WINDOW_WIDTH / 2) - BALL_RADIUS;
const float    BALL_Y        = (WINDOW_HEIGHT / 2) - BALL_RADIUS;
const float    SPEED         = 100;
///////////////////////////////
int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sans Titre", sf::Style::Close);

    Ball myBall(BALL_RADIUS, 32);
    myBall.setPosition(BALL_X, BALL_Y);
    myBall.setLimit(P1_X + BARRE_WIDTH, P2_X);

    Barre player1(BARRE_WIDTH, BARRE_HEIGHT);
    player1.setPosition(P1_X, P_Y);

    Barre player2(BARRE_WIDTH, BARRE_HEIGHT);
    player2.setPosition(P2_X, P_Y);

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
                if (event.key.code == sf::Keyboard::Z) { key[P1_UP]   = true; }
                if (event.key.code == sf::Keyboard::S) { key[P1_DOWN] = true; }
                if (event.key.code == sf::Keyboard::O) { key[P2_UP]   = true; }
                if (event.key.code == sf::Keyboard::L) { key[P2_DOWN] = true; }

                if(event.key.code == sf::Keyboard::Escape)
                    window.close();
            }

            /////// KEY RELEASED
			if (event.type == sf::Event::KeyReleased)
			{
				//Directions
                if (event.key.code == sf::Keyboard::Z) { key[P1_UP]   = false; }
                if (event.key.code == sf::Keyboard::S) { key[P1_DOWN] = false; }
                if (event.key.code == sf::Keyboard::O) { key[P2_UP]   = false; }
                if (event.key.code == sf::Keyboard::L) { key[P2_DOWN] = false; }

			}

            /////// MOUSE PRESSED
            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    myBall.setRotation(myBall.getRotation() - 10);
                    std::cout << myBall << '\n';
                }
                if(event.mouseButton.button == sf::Mouse::Right) {
                    myBall.setRotation(myBall.getRotation() + 10);
                    std::cout << myBall << '\n';
                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        /////// UPDATE
        if(key[P1_UP])   { player1.update(dt, dir::UP); }
        if(key[P2_UP])   { player2.update(dt, dir::UP); }
        if(key[P1_DOWN]) { player1.update(dt, dir::DOWN); }
        if(key[P2_DOWN]) { player2.update(dt, dir::DOWN); }

        myBall.collideWindow(&window);
        myBall.collidePaddle(player1, player2);
        myBall.update(dt);

        /////// DRAW
        window.clear();
        window.draw(player1);
        window.draw(player2);
        window.draw(myBall);
        window.draw(myBall.m_box);
        window.display();
    }

    return 0;
}
