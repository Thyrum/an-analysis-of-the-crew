#pragma once

#include "strategy/players/baseplayer.h"
#include "strategy/knowledge.h"
#include "strategy/playout-limit.h"

namespace Crew {

namespace Player {

class TrueMC : public BasePlayer {
public:
	TrueMC(PlayoutLimit limit);
	void init(const Game& game, unsigned player);
	void reset(const Game& newGame, unsigned player);

	void addTrick(const Game& game, const Trick& trick);

	Card getMove(const Game& game, unsigned player);
	int chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player);

private:
	PlayoutLimit m_limit;
	Knowledge m_know;
};

}

}
