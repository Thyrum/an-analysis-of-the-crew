#pragma once

#include "strategy/players/baseplayer.h"

namespace Crew {

namespace Player {

class Random : public BasePlayer {
public:
	Random() = default;
	Card getMove(const Game& game, unsigned player);
	int chooseTask(const Game&, const std::vector<Card>& objectives, unsigned);
};

}

}
