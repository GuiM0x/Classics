#include "../include/Cell.h"

Cell::Cell(const sf::FloatRect& rect) :
    m_alive{false},
    m_nextState{false}
{
    m_rect.setSize(sf::Vector2f(rect.width, rect.height));
    m_rect.setPosition(rect.left, rect.top);
    m_rect.setFillColor(sf::Color::Transparent);
}

void Cell::applyNextState()
{
    m_alive = m_nextState;
}

void Cell::update()
{
    (m_alive) ? m_rect.setFillColor(sf::Color::White) :
        m_rect.setFillColor(sf::Color::Transparent);
}

void Cell::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_rect);
}
