#pragma once

#include "common/card.h"
#include "common/game.h"
#include "common/trick.h"

namespace Crew {

namespace Player {

class BasePlayer {
public:
	BasePlayer() = default;
	virtual void init(const Game& game, unsigned player);
	virtual void reset(const Game& newGame, unsigned player);

	virtual void addTrick(const Game& game, const Trick& trick);

	virtual Card getMove(const Game& game, unsigned player) = 0;
	virtual int chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player) = 0;
};

}

}
