#ifndef GRILLE_H
#define GRILLE_H

#include <iostream>
#include <algorithm>
#include <memory>
#include <cassert>

#include <SFML/Graphics.hpp>

#include "Cell.h"
#include "Outils.h"

class Grille : public sf::Drawable
{
public:
	Grille() = delete;
	Grille(
        sf::RenderWindow *window,
		unsigned nb_rows,
		unsigned nb_cols,
		unsigned tile_width = 32,
		unsigned tile_height = 32
	);

	Grille(const Grille&) = delete;
	Grille& operator=(const Grille&) = delete;

	Grille(Grille&&) = default;
	Grille& operator=(Grille&&) = default;

	~Grille() = default;

	void fillWithRectangle();
	void fillWithCell();
	void switchCellByClick();
	void genereRandCells();
	void resetLife();

	void update(bool activeAutomata, const sf::Time& dt);

private:
	typedef std::vector<std::unique_ptr<sf::RectangleShape>> VectorRects;
	typedef std::vector<std::unique_ptr<Cell>> VectorCells;

	sf::RenderWindow       *m_window;
	const unsigned          m_rows;
	const unsigned          m_cols;
	const unsigned          m_tileW;
	const unsigned          m_tileH;
	const sf::Vector2u      m_map_size;
	std::size_t             m_mouseCurrIndex;
	float                   m_elapsed;
	// All container's elements are shared_ptr
	VectorRects             m_rects;
	VectorCells             m_cells;

	// Func
	void updateCellState();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void mouseCurrentIndex();
	std::size_t searchIndexByPosition(float pos_x, float pos_y) const;
	std::size_t getAliveNeighbourhood(std::size_t cellId) const;

/////// INLINE MEMBERS

};

#endif // !GRILLE_H
