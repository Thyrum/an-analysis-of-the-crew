#pragma once

#include "strategy/players/baseplayer.h"
#include "strategy/playout-limit.h"

namespace Crew {

namespace Player {

class MC : public BasePlayer {
public:
	MC(PlayoutLimit limit);
	Card getMove(const Game& game, unsigned player);
	int chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player);

private:
	PlayoutLimit m_limit;
};

}

}
