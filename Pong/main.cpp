#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "include/Outils.h"

//////////////////////////////////////////////
/////// RESET KEY STATE
void reset_key_state(std::vector<bool>& v){
    for(auto&& x : v)
        x = false;
}

enum keys{P1_UP, P1_DOWN, P2_UP, P2_DOWN, KEY_MAX};
std::vector<bool> key(KEY_MAX, false);

//////////////////////////////////////////////
/////// CONST
const unsigned WINDOW_W    = 1024;
const unsigned WINDOW_H    = 576;
const float    PADDLE_W    = 16;
const float    PADDLE_H    = 128;
const float    PLAYER1_X   = 10;
const float    PLAYER2_X   = WINDOW_W - PADDLE_W - 10;
const float    PLAYER_Y    = (WINDOW_H / 2) - (PADDLE_H / 2);
const float    BALL_RADIUS = 16;
const float    BALL_X      = (WINDOW_W / 2) - BALL_RADIUS;
const float    BALL_Y      = (WINDOW_H / 2) - BALL_RADIUS;

//////////////////////////////////////////////
/////// CLASS LEADERBOARD
class Leaderboard : public sf::Drawable
{
public:
    explicit Leaderboard(unsigned characterSize = 30) :
        m_playersScore{std::map<std::string, sf::Text>({{"player1", sf::Text()}, {"player2", sf::Text()}})},
        m_font(sf::Font()),
        m_box1(sf::RectangleShape()),
        m_box2(sf::RectangleShape())
    {
        m_font.loadFromFile("assets/fonts/OldLondon.ttf");

        for(auto it = m_playersScore.begin(); it != m_playersScore.end(); ++it) {
            it->second.setFont(m_font);
            it->second.setString("0");
            it->second.setCharacterSize(characterSize);
            it->second.setFillColor(sf::Color::White);
        }

        m_playersScore["player1"].setPosition(WINDOW_W/2.f - m_playersScore["player1"].getGlobalBounds().width, 0.f);
        m_playersScore["player2"].setPosition(WINDOW_W/2.f, 0.f);
        // Offset relative to middle
        m_playersScore["player1"].move(-30.f, 0.f);
        m_playersScore["player2"].move(30.f, 0.f);

        m_box1 = createBox(m_playersScore["player1"].getGlobalBounds().left,
                           m_playersScore["player1"].getGlobalBounds().top,
                           m_playersScore["player1"].getGlobalBounds().width,
                           m_playersScore["player1"].getGlobalBounds().height);

        m_box2 = createBox(m_playersScore["player2"].getGlobalBounds().left,
                           m_playersScore["player1"].getGlobalBounds().top,
                           m_playersScore["player2"].getGlobalBounds().width,
                           m_playersScore["player2"].getGlobalBounds().height);
    }

    void addPoint(const std::string& player)
    {
        std::string tmp{m_playersScore[player].getString()};
        int score{std::stoi(tmp)};
        ++score;
        m_playersScore[player].setString(std::to_string(score));
        if(player == "player1")
            updatePosLeftScore();
        /// DEBUG
        updateBox();
    }

private:
    //      <player     , score   >
    std::map<std::string, sf::Text> m_playersScore;
    sf::Font                        m_font;
    sf::RectangleShape              m_box1; /// DEBUG
    sf::RectangleShape              m_box2; /// DEBUG

    sf::RectangleShape createBox(float x, float y, float w, float h)
    {
        sf::RectangleShape tmp{sf::Vector2f(w, h)};
        tmp.setPosition(x, y);
        tmp.setFillColor(sf::Color::Transparent);
        tmp.setOutlineThickness(1);
        tmp.setOutlineColor(sf::Color::Red);

        return tmp;
    }

    void updatePosLeftScore()
    {
        float width{m_playersScore["player1"].getGlobalBounds().width};
        m_playersScore["player1"].setPosition((WINDOW_W/2.f) - width, 0.f);
        m_playersScore["player1"].move(-30.f, 0.f);
    }

    void updateBox()
    {
        m_box1.setPosition(m_playersScore["player1"].getGlobalBounds().left,
                           m_playersScore["player1"].getGlobalBounds().top);
        m_box2.setPosition(m_playersScore["player2"].getGlobalBounds().left,
                           m_playersScore["player2"].getGlobalBounds().top);

        m_box1.setSize(sf::Vector2f(m_playersScore["player1"].getGlobalBounds().width,
                                    m_playersScore["player1"].getGlobalBounds().height));
        m_box2.setSize(sf::Vector2f(m_playersScore["player2"].getGlobalBounds().width,
                                    m_playersScore["player2"].getGlobalBounds().height));
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        //target.draw(m_box1);
        //target.draw(m_box2);
        for(auto it = m_playersScore.begin(); it != m_playersScore.end(); ++it) {
            target.draw(it->second);
        }
    }
};

//////////////////////////////////////////////
/////// CLASS BARRE
enum dir{
    UP   = -1,
    DOWN = 1
};

class Paddle : public sf::RectangleShape
{
public:
    Paddle(float width, float height) :
        RectangleShape(sf::Vector2f(width, height)),
        m_texture(sf::Texture())
    {
        m_texture.loadFromFile("assets/img/paddle.png");
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
//////////////////////////////////////////////
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
        m_texture.loadFromFile("assets/img/ball.png");
        m_texture.setSmooth(true);
        setTexture(&m_texture);
        m_texture_cracked.loadFromFile("assets/img/ball_cracked.png");
        m_texture_cracked.setSmooth(true);
        // Sounds
        m_bufferSoundCrack.loadFromFile("assets/sounds/sound_crack.wav");
        m_soundCrack.setBuffer(m_bufferSoundCrack);
        m_bufferSoundBounce.loadFromFile("assets/sounds/sound_bounce.wav");
        m_soundBounce.setBuffer(m_bufferSoundBounce);
        // Origin point (for perfect rotation in middle of texture)
        setOrigin(radius, radius);
        /// View Collide box (draw not necessary, but box's datas are used in collide sys)
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

    void speedUp() {
        m_speed += 30.f;
        if(m_speed < 0)
            m_speed = 0.f;
    }

    /// DEBUG
    void slowDown() {
        m_speed -= 30.f;
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
        setPosition(WINDOW_W/2.f, WINDOW_H/2.f);
        m_box.setPosition((WINDOW_W/2.f) - getRadius(), (WINDOW_H/2.f) - getRadius());
        startRotation();
        m_speed = m_defaultSpeed;
        m_isOut = false;
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
/// DEBUG
std::ostream& operator<<(std::ostream& os, const Ball& b)
{
    os << "Rotation : " << b.getRotation() << " deg(s)" << '\n'
       << "Speed    : " << b.getSpeed() << '\n'
       << "-------------------------";
    return os;
}
//////////////////////////////////////////////
/////// GET ANGLE REFLEXION
float getAngleReflexion(Ball& b, const Paddle& p)
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
//////////////////////////////////////////////
/////// COLLIDE WINDOW
bool collideWindow(Ball& b, sf::RenderTarget *window)
{
    // Top
    if(b.top() <= b.getRadius()) {
        b.setPosition(b.left(), b.getRadius());
        b.bounceH();
        return true;
    }
    // Bottom
    if(b.top() >= (window->getSize().y - b.getRadius())) {
        b.setPosition(b.left(), window->getSize().y - b.getRadius());
        b.bounceH();
        return true;
    }
    //Left - Right
    if(b.left() <= b.getRadius() || b.left() >= window->getSize().x - b.getRadius()) {
        if(!b.isOut())
            b.playCrackSound();
        b.setOut(true);
        return true;
    }

    return false;
}
//////////////////////////////////////////////
/////// COLLIDE PADDLE
void collidePaddle(Ball& b, const Paddle& p1, const Paddle& p2)
{
    const sf::FloatRect player1(p1.getGlobalBounds());
    const sf::FloatRect player2(p2.getGlobalBounds());
    const float halfBarre{p1.getGlobalBounds().height / 2.f};
    const float ballCenter{b.getPosition().y};
    const float p1Center{p1.top() + halfBarre};
    const float p2Center{p2.top() + halfBarre};

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

//////////////////////////////////////////////
int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "Pong", sf::Style::Close);

    // BALL
    Ball myBall(BALL_RADIUS, 32);
    myBall.setPosition(BALL_X, BALL_Y);
    myBall.setLimit(PLAYER1_X + PADDLE_W, PLAYER2_X);

    // PLAYER 1
    Paddle player1(PADDLE_W, PADDLE_H);
    player1.setPosition(PLAYER1_X, PLAYER_Y);

    // PLAYER 2
    Paddle player2(PADDLE_W, PADDLE_H);
    player2.setPosition(PLAYER2_X, PLAYER_Y);

    // LEADERBOARD
    Leaderboard leaderboard(75);

    // DOT LINE
    std::vector<sf::RectangleShape> middleDotLine(9, sf::RectangleShape{sf::Vector2f(10.f, WINDOW_H / 18.f)});
    for(std::size_t i = 0; i < middleDotLine.size(); ++i) {
        middleDotLine[i].setPosition((WINDOW_W / 2.f) - 2.f, (i*(WINDOW_H / 9.f)) + WINDOW_H / 36.f);
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
        if(!myBall.isOut()) {
            // Collide Window
            if(collideWindow(myBall, &window)) {
                if(myBall.left() <= myBall.getRadius())
                    leaderboard.addPoint("player2");
                else if(myBall.right() >= WINDOW_W)
                    leaderboard.addPoint("player1");
            }
            // Collide Paddle
            collidePaddle(myBall, player1, player2);

            // Speed Ball
            if(timer > 3.f) {
                myBall.speedUp();
                timer = 0.f;
            }
        }
        else {
            timer = 0.f;
        }

        // Players
        if(key[P1_UP])   { player1.update(dt, dir::UP); }
        if(key[P2_UP])   { player2.update(dt, dir::UP); }
        if(key[P1_DOWN]) { player1.update(dt, dir::DOWN); }
        if(key[P2_DOWN]) { player2.update(dt, dir::DOWN); }

        // Ball
        myBall.update(dt);

        /////// DRAW
        window.clear();
        for(const auto& x: middleDotLine)
            window.draw(x);
        window.draw(player1);
        window.draw(player2);
        window.draw(myBall);
        //window.draw(myBall.getBox()); /// DEBUG
        window.draw(leaderboard);
        window.display();
    }

    return 0;
}
