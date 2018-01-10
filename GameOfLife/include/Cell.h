#ifndef CELL_H
#define CELL_H

#include <SFML/Graphics.hpp>

class Cell : public sf::Drawable
{
public:
    Cell() = delete;
    Cell(const sf::FloatRect&);

    Cell(const Cell&) = delete;
    Cell& operator=(const Cell&) = delete;
    Cell(Cell&&) = default;
    Cell& operator=(Cell&&)= default;

    ~Cell() = default;

    inline void setAlive(bool alive) { m_alive = alive; }
    inline bool isAlive() const { return m_alive; }
    inline void setNextState(bool alive) { m_nextState = alive; }
    void applyNextState();
    void update();

private:
    bool               m_alive;
    bool               m_nextState;
    sf::RectangleShape m_rect;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif // CELL_H
