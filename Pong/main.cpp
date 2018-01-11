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
        RectangleShape(sf::Vector2f(width, height)) {}

    float top() const { return getGlobalBounds().top; }
    float bottom() const { return (getGlobalBounds().top + getGlobalBounds().height); }
    float left() const { return getGlobalBounds().left; }
    float right() const { return (getGlobalBounds().left + getGlobalBounds().width); }

    void update(const sf::Time& dt, int direction) {
        setPosition(getPosition().x, (getPosition().y + (m_speed * dt.asSeconds() * direction)));
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
        m_box(sf::RectangleShape(sf::Vector2f(radius*2, radius*2))),
        m_texture(sf::Texture())
    {
        m_texture.loadFromFile("assets/arrow_ball.png");
        m_texture.setSmooth(true);
        setTexture(&m_texture);
        setOrigin(radius, radius);
        m_box.setFillColor(sf::Color::Transparent);
        m_box.setOutlineThickness(1);
        m_box.setOutlineColor(sf::Color::Red);
        //setRotation(Outils::rollTheDice(1, 359));
    }

    float top() const { return getPosition().y; }
    float bottom() const { return (getPosition().y + getGlobalBounds().height); }
    float left() const { return getPosition().x; }
    float right() const { return (getGlobalBounds().left + getGlobalBounds().width); }
    float getSpeed() const { return m_speed; }
    sf::FloatRect getFloatRect() { return m_box.getGlobalBounds(); }
    const sf::RectangleShape& getBox() { return m_box; }

    void setLimit(float left, float right) {
        m_limitLeft = left;
        m_limitRight = right;
    }

    void bounceH() { setRotation(360 - getRotation()); }
    void bounceV() { setRotation(90 - (getRotation() - 90)); }

    void update(const sf::Time& dt)
    {
        float radA = getRotation() * ((2.f * PI) / 360.f);
        float x    = left() + (cos(radA) * m_speed * dt.asSeconds());
        float y    = top() + (sin(radA) * m_speed * dt.asSeconds());
        setPosition(x, y);
        m_box.setPosition(x - getRadius(), y - getRadius());
    }

    /// DEBUG
    void slowDown() {
        m_speed -= 15.f;
        if(m_speed < 0)
            m_speed = 0.f;
    }
    void speedUp() {
        m_speed += 15.f;
        if(m_speed < 0)
            m_speed = 0.f;
    }

private:
    sf::RectangleShape m_box;
    sf::Texture        m_texture;
    const float        PI           = 3.141592f;
    float              m_speed      = 100.f;
    float              m_limitLeft  = 0.f;
    float              m_limitRight = 0.f;
};

std::ostream& operator<<(std::ostream& os, const Ball& b)
{
    os << "Rotation : " << b.getRotation() << " deg(s)" << '\n'
       << "Speed    : " << b.getSpeed() << '\n'
       << "-------------------------";
    return os;
}
///////////////////////////////
/////// FUNC
void collideWindow(Ball& b, sf::RenderTarget *window)
{
    // Top
    if(b.top() <= b.getRadius()) {
        b.setPosition(b.left(), b.getRadius());
        b.bounceH();
    }
    // Bottom
    if(b.top() >= (window->getSize().y - b.getRadius())) {
        b.setPosition(b.left(), window->getSize().y - b.getRadius());
        b.bounceH();
    }
    //Left - Right
    if(b.left() <= b.getRadius() || b.left() >= window->getSize().x - b.getRadius()) {
        b.bounceV(); // TO DO : point pour l'adversaire selon côté
    }
}

void collidePaddle(Ball& b, const Barre& p1, const Barre& p2)
{
    sf::FloatRect player1(p1.getGlobalBounds());
    sf::FloatRect player2(p2.getGlobalBounds());

    // If Ball is inside player 1
    if(player1.intersects(b.getFloatRect())) {

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

        if(b.top() <= p1.top() && b.bottom() >= p1.top()) {
            //Corner top
            // if come from down
            if(b.getRotation() > 180 && b.getRotation() < 270)
                b.bounceV();
            else
                b.bounceH();
        }
        else if(b.top() - b.getRadius() <= p1.bottom() && b.bottom() >= p1.bottom()) {
            //Corner bottom
            // if come from up
            if(b.getRotation() > 90 && b.getRotation() < 180)
                b.bounceV();
            else
                b.bounceH();
        }
        else {
            b.bounceV();
        }
    }

    // If Ball is inside player 2
    if(player2.intersects(b.getFloatRect())) {

        if(b.top() <= p2.top() && b.bottom() >= p2.top()) {
            // Corner top
            // if ball comes from down
            if(b.getRotation() > 270 && b.getRotation() < 360)
                b.bounceV();
            else
                b.bounceH();
        }
        else if(b.top() - b.getRadius() <= p2.bottom() && b.bottom() >= p2.bottom()) {
            //Corner bottom
            // if ball comes from up
            if(b.getRotation() > 0 && b.getRotation() < 90)
                b.bounceV();
            else
                b.bounceH();
        }
        else {
            b.bounceV();
        }
    }
}

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
    float timer{0.f};

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
                }
                if(event.mouseButton.button == sf::Mouse::Right) {
                    myBall.setRotation(myBall.getRotation() + 10);
                }
            }

            /////// MOUSE WHEEL
            if(event.type == sf::Event::MouseWheelScrolled) {
                if(event.mouseWheelScroll.delta > 0) {
                    myBall.speedUp();
                }
                if(event.mouseWheelScroll.delta < 0) {
                    myBall.slowDown();
                }
            }

            /////// CLOSE WINDOW
            if (event.type == sf::Event::Closed)
                window.close();
        }

        /// DEBUG
        if(timer > 1.f) {
            timer = 0.f;
            myBall.setRotation(Outils::rollTheDice(1, 360));
            std::cout << myBall << '\n';
        }

        /////// UPDATE
        if(key[P1_UP])   { player1.update(dt, dir::UP); }
        if(key[P2_UP])   { player2.update(dt, dir::UP); }
        if(key[P1_DOWN]) { player1.update(dt, dir::DOWN); }
        if(key[P2_DOWN]) { player2.update(dt, dir::DOWN); }

        collideWindow(myBall, &window);
        collidePaddle(myBall, player1, player2);
        myBall.update(dt);

        /////// DRAW
        window.clear();
        window.draw(player1);
        window.draw(player2);
        window.draw(myBall);
        window.draw(myBall.getBox());
        window.display();
    }

    return 0;
}
