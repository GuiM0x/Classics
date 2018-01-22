#include "../include/Grille.h"

Grille::Grille(
    sf::RenderWindow *window,
	unsigned nb_rows,
	unsigned nb_cols,
	unsigned tile_width,
	unsigned tile_height
) :
    m_window(window),
    m_rows{nb_rows},
    m_cols{nb_cols},
    m_tileW{tile_width},
    m_tileH{tile_height},
    m_map_size(sf::Vector2u(m_cols*m_tileW, m_rows*m_tileH)),
    m_mouseCurrIndex{0},
    m_elapsed{0},
    m_rects(std::vector<std::unique_ptr<sf::RectangleShape>>()),
    m_cells(std::vector<std::unique_ptr<Cell>>())
{

}

////////// FILL WITH RECT
void Grille::fillWithRectangle()
{
	std::size_t id{0};

	for (unsigned i = 0; i < m_rows; ++i) {
		for (unsigned j = 0; j < m_cols; ++j) {
			m_rects.push_back(std::make_unique<sf::RectangleShape>(
				sf::Vector2f(
					static_cast<float>(m_tileW),
					static_cast<float>(m_tileH))
				));
			m_rects[id]->setFillColor(sf::Color::Transparent);
			m_rects[id]->setOutlineThickness(1);
			m_rects[id]->setOutlineColor(sf::Color(85, 85, 85, 100));
			m_rects[id]->setPosition(static_cast<float>(j * m_tileW), static_cast<float>(i * m_tileH));
			++id;
		}
	}
}

////////// FILL WITH CELL
void Grille::fillWithCell()
{
	for (unsigned i = 0; i < m_rows; ++i) {
		for (unsigned j = 0; j < m_cols; ++j) {
			m_cells.push_back(std::make_unique<Cell>(
				sf::FloatRect(
                    static_cast<float>(j * m_tileW),
                    static_cast<float>(i * m_tileH),
                    static_cast<float>(m_tileW),
                    static_cast<float>(m_tileH)
                  )));
		}
	}
}

////////// SWITCH CELL BY CLICK
void Grille::switchCellByClick()
{
    if(m_mouseCurrIndex > m_cells.size()-1)
        m_mouseCurrIndex = 0;

    (m_cells[m_mouseCurrIndex]->isAlive()) ?
        m_cells[m_mouseCurrIndex]->setAlive(false) :
            m_cells[m_mouseCurrIndex]->setAlive(true);
}

////////// RESET LIFE
void Grille::resetLife()
{
    for(auto&& x : m_cells) {
        x->setAlive(false);
        x->setNextState(false);
    }
}

////////// GENERE RAND
void Grille::genereRandCells()
{
    resetLife();
    for(auto&& x : m_cells) {
        bool dice = static_cast<bool>(Outils::rollTheDice(0, 1));
        x->setAlive(dice);
    }
}

/////// SEARCH INDEX BY POSITION
std::size_t Grille::searchIndexByPosition(float pos_x, float pos_y) const
{
	int row = static_cast<int>(pos_y / m_tileH);
	int col = static_cast<int>(pos_x / m_tileW);

	return static_cast<std::size_t>(col + (row * m_cols));
}

////////// MOUSE CURRENT INDEX
void Grille::mouseCurrentIndex()
{
	float x = static_cast<float>(sf::Mouse::getPosition(*m_window).x);
	float y = static_cast<float>(sf::Mouse::getPosition(*m_window).y);

	m_mouseCurrIndex = searchIndexByPosition(x, y);

	if (m_mouseCurrIndex >= (m_rows * m_cols))
		m_mouseCurrIndex = 0;
}

////////// GET ALIVE NEIGHGBOURHOOD
std::size_t Grille::getAliveNeighbourhood(std::size_t cellId) const
{
    std::vector<std::size_t> tmp_id;
    tmp_id.push_back((cellId - 1) - m_cols); // topLeft
    tmp_id.push_back(cellId - m_cols); // top
    tmp_id.push_back((cellId + 1) - m_cols); // topRight
    tmp_id.push_back(cellId - 1); // left
    tmp_id.push_back(cellId + 1); // right
    tmp_id.push_back((cellId - 1) + m_cols); // bottomLeft
    tmp_id.push_back(cellId + m_cols); // bottom
    tmp_id.push_back((cellId + 1) + m_cols); // bottomRight

    std::size_t nb_neighbour{0};

    for(const auto& x : tmp_id) {
        if(x >= 0 && x <= m_cells.size()-1 && m_cells[x]->isAlive())
            ++nb_neighbour;
    }

    return nb_neighbour;
}

////////// UPDATE NEIGHBOURHOOD
void Grille::updateCellState()
{
    int index{0};

    for(auto&& x : m_cells){
        std::size_t nb_around_live{getAliveNeighbourhood(index)};
        if(x->isAlive()){
            (nb_around_live == 2 || nb_around_live == 3) ?
                x->setNextState(true) : x->setNextState(false);
        }
        else {
            if(nb_around_live == 3)
                x->setNextState(true);
        }
        ++index;
    }

    for(auto&& x : m_cells){
        x->applyNextState();
    }
}

////////// UPDATE
void Grille::update(bool activeAutomata, const sf::Time& dt)
{
	mouseCurrentIndex();
    m_elapsed += dt.asSeconds();
	if(activeAutomata && m_elapsed > 0.1){
        updateCellState();
        m_elapsed = 0;
	}

	if(!m_cells.empty()){
        for(auto&& x : m_cells)
            x->update();
	}
}

////////// DRAW
void Grille::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for(const auto& x :m_cells)
        target.draw(*x);

    for (const auto& x : m_rects)
        target.draw(*x);
}
