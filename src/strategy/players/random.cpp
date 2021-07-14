#include "strategy/players/random.h"


#include "strategy/util.h"

namespace Crew {

namespace Player {

Card Random::getMove(const Game& game, unsigned player) {
	std::vector<Card> playable = getPlayable(game, player);
	int card = std::rand() % playable.size();
	return playable[card];
}

int Random::chooseTask(const Game&, const std::vector<Card>& objectives, unsigned) {
	return std::rand() % objectives.size();
}

}

}
