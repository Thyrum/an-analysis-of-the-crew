#pragma once

#include "strategy/players/baseplayer.h"

namespace Crew {

namespace Player {

class User : public BasePlayer {
public:
	User() = default;
	Card getMove(const Game& game, unsigned player);
	int chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player);
};

}

}
