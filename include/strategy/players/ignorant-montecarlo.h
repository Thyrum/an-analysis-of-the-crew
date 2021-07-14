#pragma once

#include "strategy/players/baseplayer.h"
#include "strategy/playout-limit.h"

namespace Crew {

namespace Player {

class IgnorantMC : public BasePlayer {
public:
	IgnorantMC(PlayoutLimit limit);
	Card getMove(const Game& game, unsigned player);
	int chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player);

private:
	std::vector<std::vector<Card>> generateHands(
		const Game& game,
		unsigned player,
		std::vector<Card> otherPlayerCards
	);
	std::vector<Card> getOtherPlayerCards(
		const Game& game,
		unsigned player
	);

	PlayoutLimit m_limit;
};

}

}
