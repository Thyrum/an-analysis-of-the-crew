#pragma once

#include <vector>
#include <iostream>

#include "common/card.h"
#include "common/trick.h"
#include "common/status.h"

namespace Crew {

struct Game {
	Game(
		std::vector<std::vector<Card>> handsIn,
		std::vector<std::vector<Card>> objectivesIn,
		unsigned int suitCount,
		unsigned int commander
	);

	Game();

	unsigned int H(unsigned int p) const;
	unsigned currentPlayer() const;

	unsigned int P,S,T,HMax;
	unsigned int c;
	std::vector<std::vector<Card>> hands;
	std::vector<std::vector<Card>> objectives;

	Trick currentTrick;

	// If currentTrick.completed(), update objectives and return status,
	// else, just return status.
	Status getStatus() const;
	Status applyTrick();
	Status playCard(const Card& card);

	void dump(std::ostream& os = std::cout) const;
};

}

