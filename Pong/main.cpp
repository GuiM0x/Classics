#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

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
        RectangleShape(sf::Vector2f(width, height)),
        m_texture(sf::Texture())
    {
        m_texture.loadFromFile("assets/paddle.png");
        m_texture.setSmooth(true);
        setTexture(&m_texture);
    }

    float top()    const { return getGlobalBounds().top; }
    float bottom() const { return (getGlobalBounds().top + getGlobalBounds().height); }
    float left()   const { return getGlobalBounds().left; }
    float right()  const { return (getGlobalBounds().left + getGlobalBounds().width); }

    void update(const sf::Time& dt, int direction) {
        setPosition(getPosition().x, (getPosition().y + (m_speed * dt.asSeconds() * direction)));
    }

private:
    sf::Texture m_texture;
    float       m_speed = 250.f;
};
///////////////////////////////
/////// CLASS BALL
class Ball : public sf::CircleShape
{
public:
    explicit Ball(float radius = 0, std::size_t pointCount = 30) :
        CircleShape(radius, pointCount),
        m_box(sf::RectangleShape(sf::Vector2f(radius*2, radius*2))),
        m_texture(sf::Texture()),
        m_texture_cracked(sf::Texture()),
        m_bufferSoundCrack(sf::SoundBuffer())
    {
        // Textures
        m_texture.loadFromFile("assets/ball.png");
        m_texture.setSmooth(true);
        setTexture(&m_texture);
        m_texture_cracked.loadFromFile("assets/ball_cracked.png");
        m_texture_cracked.setSmooth(true);
        // Sounds
        m_bufferSoundCrack.loadFromFile("sounds/sound_crack.wav");
        m_soundCrack.setBuffer(m_bufferSoundCrack);
        m_bufferSoundBounce.loadFromFile("sounds/sound_bounce.wav");
        m_soundBounce.setBuffer(m_bufferSoundBounce);
        // Origin point (for perfect rotation in middle of texture)
        setOrigin(radius, radius);
        // Collide box
        m_box.setFillColor(sf::Color::Transparent);
        m_box.setOutlineThickness(1);
        m_box.setOutlineColor(sf::Color::Red);
        startRotation();
    }

    float top() const                  { return getPosition().y; }
    float bottom() const               { return (getPosition().y + getGlobalBounds().height); }
    float left() const                 { return getPosition().x; }
    float right() const                { return (getGlobalBounds().left + getGlobalBounds().width); }
    float getSpeed() const             { return m_speed; } /// DEBUG
    float limitLeft() const            { return m_limitLeft; }
    float limitRight() const           { return m_limitRight; }
    bool isOut() const                 { return m_isOut; }
    sf::FloatRect getFloatRect()       { return m_box.getGlobalBounds(); }
    const sf::RectangleShape& getBox() { return m_box; }

    void setLimit(float left, float right) {
        m_limitLeft = left;
        m_limitRight = right;
    }

    void setOut(bool isOut) { m_isOut = isOut; }
    void playCrackSound()   { m_soundCrack.play(); }
    void playBounceSound()  { m_soundBounce.play(); }
    void bounceH()          { setRotation(360 - getRotation()); }
    //void bounceV()          { setRotation(90 - (getRotation() - 90)); }

    void update(const sf::Time& dt)
    {
        if(!m_isOut) {
            float radA = getRotation() * ((2.f * PI) / 360.f);
            float x    = left() + (cos(radA) * m_speed * dt.asSeconds());
            float y    = top() + (sin(radA) * m_speed * dt.asSeconds());
            setPosition(x, y);
            m_box.setPosition(x - getRadius(), y - getRadius());
        }
        else {
            playAnimOut(dt);
        }
    }

    /// DEBUG
    void slowDown() {
        m_speed -= 30.f;
        if(m_speed < 0)
            m_speed = 0.f;
    }
    void speedUp() {
        m_speed += 30.f;
        if(m_speed < 0)
            m_speed = 0.f;
    }

private:
    sf::RectangleShape m_box;
    sf::Texture        m_texture;
    sf::Texture        m_texture_cracked;
    sf::SoundBuffer    m_bufferSoundCrack;
    sf::Sound          m_soundCrack;
    sf::SoundBuffer    m_bufferSoundBounce;
    sf::Sound          m_soundBounce;
    const float        PI             = 3.141592f;
    float              m_defaultSpeed = 300.f;
    float              m_speed        = 300.f;
    float              m_limitLeft    = 0.f;
    float              m_limitRight   = 0.f;
    float              m_elapsed      = 0.f;
    bool               m_isOut        = false;

    void reset()
    {
        m_speed = m_defaultSpeed;
        setPosition(1024/2.f, 576/2.f);
        m_isOut = false;
        startRotation();
    }

    void startRotation()
    {
        float angle{static_cast<float>(Outils::rollTheDice(0, 359))};

        if((angle > 225.f && angle < 315.f) ||
           (angle > 45.f && angle < 135.f))
        {
            angle -= 90.f;
        }

        setRotation(angle);
    }

    void playAnimOut(const sf::Time& dt)
    {
        m_elapsed += dt.asSeconds();
        if(m_elapsed < 2.f) {
            setTexture(&m_texture_cracked);
        }
        else {
            setTexture(&m_texture);
            reset();
            m_elapsed = 0.f;
        }
    }
};

std::ostream& operator<<(std::ostream& os, const Ball& b)
{
    os << "Rotation : " << b.getRotation() << " deg(s)" << '\n'
       << "Speed    : " << b.getSpeed() << '\n'
       << "-------------------------";
    return os;
}
///////////////////////////////
/////// GET ANGLE REFLEXION
float getAngleReflexion(Ball& b, const Barre& p)
{
    const float angleMax{45.f};
    const float halfBarre{p.getGlobalBounds().height / 2.f};
    const float degPerPxl{angleMax / halfBarre};
    const float ballCenter{b.getPosition().y};
    const float barreCenter{p.getPosition().y + halfBarre};
    float angle{0.f};

    if(ballCenter < barreCenter) {
        return ((barreCenter - ballCenter) * degPerPxl);
    }
    else if(ballCenter > barreCenter) {
        return ((ballCenter - barreCenter) * degPerPxl);
    }
    else {
        return angle;
    }
}
///////////////////////////////
/////// COLLIDE WINDOW
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
        if(!b.isOut())
            b.playCrackSound();
        b.setOut(true);
    }
}
///////////////////////////////
/////// COLLIDE PADDLE
void collidePaddle(Ball& b, const Barre& p1, const Barre& p2)
{
    if(!b.isOut()) {
        sf::FloatRect player1(p1.getGlobalBounds());
        sf::FloatRect player2(p2.getGlobalBounds());
        float halfBarre{p1.getGlobalBounds().height / 2.f};
        float ballCenter{b.getPosition().y};
        float p1Center{p1.top() + halfBarre};
        float p2Center{p2.top() + halfBarre};

        // If Ball is inside player 1
        if(player1.intersects(b.getFloatRect())) {

            b.playBounceSound();

            if(ballCenter < p1Center) {
                b.setRotation(360.f - getAngleReflexion(b, p1));
            }
            else if(ballCenter > p1Center) {
                b.setRotation(getAngleReflexion(b, p1));
            }
            else {
                b.setRotation(0.f);
            }
        }

        // If Ball is inside player 2
        if(player2.intersects(b.getFloatRect())) {

            b.playBounceSound();

            if(ballCenter < p2Center) {
                b.setRotation(180.f + getAngleReflexion(b, p2));
            }
            else if(ballCenter > p2Center) {
                b.setRotation(180.f - getAngleReflexion(b, p2));
            }
            else {
                b.setRotation(0.f);
            }
        }
    }
}

///////////////////////////////
/////// CONST
const unsigned WINDOW_WIDTH  = 1024;
const unsigned WINDOW_HEIGHT = 576;
const float    BARRE_WIDTH   = 16;
const float    BARRE_HEIGHT  = 128;
const float    P1_X          = 10;
const float    P2_X          = WINDOW_WIDTH - BARRE_WIDTH - 10;
const float    P_Y           = (WINDOW_HEIGHT / 2) - (BARRE_HEIGHT / 2);
const float    BALL_RADIUS   = 16;
const float    BALL_X        = (WINDOW_WIDTH / 2) - BALL_RADIUS;
const float    BALL_Y        = (WINDOW_HEIGHT / 2) - BALL_RADIUS;
///////////////////////////////
int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sans Titre", sf::Style::Close);

    // BALL
    Ball myBall(BALL_RADIUS, 32);
    myBall.setPosition(BALL_X, BALL_Y);
    myBall.setLimit(P1_X + BARRE_WIDTH, P2_X);

    // PLAYER 1
    Barre player1(BARRE_WIDTH, BARRE_HEIGHT);
    player1.setPosition(P1_X, P_Y);

    // PLAYER 2
    Barre player2(BARRE_WIDTH, BARRE_HEIGHT);
    player2.setPosition(P2_X, P_Y);

    // DOT LINE
    std::vector<sf::RectangleShape> middleDotLine(9,
                                                  sf::RectangleShape(sf::Vector2f(10.f, WINDOW_HEIGHT / 18.f)));
    for(std::size_t i = 0; i < middleDotLine.size(); ++i) {
        middleDotLine[i].setPosition((WINDOW_WIDTH / 2.f) - 5.f,
                                     (i*(WINDOW_HEIGHT / 9.f)) + WINDOW_HEIGHT / 36.f);
    }

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

                /// DEBUG
                if (event.key.code == sf::Keyboard::D) {
                    std::cout << myBall << '\n';
                }

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

        /////// UPDATE
        // Collide
        collideWindow(myBall, &window);
        collidePaddle(myBall, player1, player2);
        // Players
        if(key[P1_UP])   { player1.update(dt, dir::UP); }
        if(key[P2_UP])   { player2.update(dt, dir::UP); }
        if(key[P1_DOWN]) { player1.update(dt, dir::DOWN); }
        if(key[P2_DOWN]) { player2.update(dt, dir::DOWN); }
        // Ball
        myBall.update(dt);
        // SpeedBall
        if(!myBall.isOut()) {
            if(timer > 3.f) {
                myBall.speedUp();
                timer = 0.f;
            }
        }
        else {
            timer = 0.f;
        }

        /////// DRAW
        window.clear();
        for(const auto& x: middleDotLine)
            window.draw(x);
        window.draw(player1);
        window.draw(player2);
        window.draw(myBall);
        window.draw(myBall.getBox());
        window.display();
    }

    return 0;
}
