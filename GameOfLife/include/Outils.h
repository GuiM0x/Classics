#ifndef OUTILS_H
#define OUTILS_H

#include <ctime>
#include <random>

namespace Outils{
	/////////// DICE-ROLL GENERATOR
	inline int rollTheDice(int valmin, int valmax)
	{
		static std::mt19937 generator{static_cast<unsigned>(time(nullptr))};
		std::uniform_int_distribution<> dist{valmin, valmax};
		return dist(generator);
	}
}

#endif // OUTILS_H
