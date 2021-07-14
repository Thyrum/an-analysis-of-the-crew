#include "common/game.h"

namespace Crew {

void randomize(Game& game);
void randomize(Game& game, size_t objectiveCount);

std::vector<std::vector<Card>> generateHands(
	unsigned int players,
	unsigned int suits,
	unsigned int suitCards,
	unsigned int trumpCards
);

}
